import requests
from requests.adapters import HTTPAdapter, Retry
from typing import Any, Dict, Optional


class LazarusClientError(Exception):
    """Simple exception for Lazarus SDK errors."""

    def __init__(self, http_status: int, lazarus_status: Optional[int], host: str, port: int):
        self.http_status = http_status
        self.lazarus_status = lazarus_status
        self.host = host
        self.port = port

    def __str__(self) -> str:
        parts = [f"LazarusClientError"]
        if self.http_status is not None:
            parts.append(f"HTTP={self.http_status}")
        if self.lazarus_status is not None:
            parts.append(f"Lazarus=0x{self.lazarus_status:X}")
        parts.append(f"Server={self.host}:{self.port}")
        return " ".join(parts)


class LazarusClient:
    """Python SDK for Lazarus Data Store."""

    def __init__(self, port: int, host: str = "http://localhost", timeout: int = 10):
        self.host = host
        self.port = port
        self.base_url = f"{host}:{port}/lazarus/network"
        self.timeout = timeout

        self.session = requests.Session()
        retries = Retry(
            total=5,
            backoff_factor=0.3,
            status_forcelist=(500, 502, 503, 504),
            allowed_methods=["GET", "POST", "DELETE", "PUT"],
        )
        adapter = HTTPAdapter(max_retries=retries)
        self.session.mount("http://", adapter)
        self.session.mount("https://", adapter)

    def _request(self, method: str, endpoint: str, **kwargs) -> Any:
        url = f"{self.base_url}/{endpoint}"
        try:
            response = self.session.request(method, url, timeout=self.timeout, **kwargs)
            response.raise_for_status()

            if response.headers.get("Content-Type", "").startswith("application/json"):
                return response.json()
            return response.text

        except requests.exceptions.HTTPError:
            # Try to parse Lazarus internal status
            lazarus_status = None
            try:
                error_json = response.json()
                if "internal_status_code" in error_json:
                    # Convert "0x..." string to int
                    lazarus_status = int(error_json["internal_status_code"], 16)
                elif "status_code" in error_json:
                    lazarus_status = int(error_json["status_code"])
            except Exception:
                pass

            raise LazarusClientError(
                http_status=response.status_code,
                lazarus_status=lazarus_status,
                host=self.host,
                port=self.port,
            )

        except requests.exceptions.RequestException:
            # Network-level errors
            raise LazarusClientError(
                http_status=None,
                lazarus_status=None,
                host=self.host,
                port=self.port,
            )

    # -------------------
    # Container operations
    # -------------------
    def create_container(self, container_name: str) -> Dict[str, Any]:
        return self._request("PUT", "container_endpoint/", json={"container_name": container_name})

    def delete_container(self, container_name: str) -> Dict[str, Any]:
        return self._request("DELETE", "container_endpoint/", json={"container_name": container_name})

    # ----------------
    # Object operations
    # ----------------
    def insert_object(self, container_name: str, object_id: str, data: str) -> Dict[str, Any]:
        payload = {"container_name": container_name, "object_id": object_id, "object_data": data}
        return self._request("POST", "object_endpoint/", json=payload)

    def get_object(self, container_name: str, object_id: str) -> Optional[str]:
        payload = {"container_name": container_name, "object_id": object_id}
        return self._request("GET", "object_endpoint/", json=payload)

    def delete_object(self, container_name: str, object_id: str) -> Dict[str, Any]:
        payload = {"container_name": container_name, "object_id": object_id}
        return self._request("DELETE", "object_endpoint/", json=payload)

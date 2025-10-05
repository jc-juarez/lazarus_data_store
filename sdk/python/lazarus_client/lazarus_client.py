import requests
from typing import Any, Dict, Optional


class LazarusClientError(Exception):
    """Represents an error from the Lazarus SDK or server."""

    def __init__(self, http_status: Optional[int], lazarus_status: Optional[int], host: str, port: int):
        self.http_status = http_status
        self.lazarus_status = lazarus_status
        self.host = host
        self.port = port

    def __str__(self) -> str:
        parts = ["LazarusClientError"]
        if self.http_status is not None:
            parts.append(f"HTTP={self.http_status}")
        if self.lazarus_status is not None:
            parts.append(f"Lazarus=0x{self.lazarus_status:X}")
        parts.append(f"Server={self.host}:{self.port}")
        return " ".join(parts)


class LazarusClient:
    """Python client for the Lazarus Data Store."""

    def __init__(self, host: str, port: int, timeout: int = 10):
        self.host = host
        self.port = port
        self.base_url = f"http://{host}:{port}/lazarus/network"
        self.timeout = timeout
        self.session = requests.Session()

    def _request(self, method: str, endpoint: str, **kwargs) -> Any:
        url = f"{self.base_url}/{endpoint}"
        try:
            response = self.session.request(method, url, timeout=self.timeout, **kwargs)
            response.raise_for_status()

            try:
                return response.json()
            except ValueError:
                return response.text

        except requests.exceptions.HTTPError:
            lazarus_status = None
            try:
                data = response.json()
                lazarus_status = int(data.get("internal_status_code") or data.get("status_code") or 0)
            except Exception:
                pass

            raise LazarusClientError(
                response.status_code,
                lazarus_status,
                self.host,
                self.port,
            ) from None

        except requests.exceptions.RequestException:
            raise LazarusClientError(None, None, self.host, self.port) from None

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
        """Fetch an object’s data from the store.

        If the server returns a JSON like:
          {"internal_status_code": "0x0", "object_data": "hello world"}
        this will return only the 'object_data' field.
        """
        payload = {"container_name": container_name, "object_id": object_id}
        result = self._request("GET", "object_endpoint/", json=payload)

        # Normalize common Lazarus response format
        if isinstance(result, dict):
            # Prefer the object_data field if present
            if "object_data" in result:
                return result["object_data"]
            # Fallback: return as-is if no recognizable key
            return result

        return result

    def delete_object(self, container_name: str, object_id: str) -> Dict[str, Any]:
        payload = {"container_name": container_name, "object_id": object_id}
        return self._request("DELETE", "object_endpoint/", json=payload)

    # ----------------
    # Service operations
    # ----------------
    def ping(self) -> bool:
        """Simple liveness check for the Lazarus service."""
        try:
            result = self._request("GET", "ping_endpoint/")
            return True if result else False
        except LazarusClientError:
            return False
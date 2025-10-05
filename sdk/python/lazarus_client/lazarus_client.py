# ****************************************************
# Lazarus Data Store
# SDK
# 'lazarus_client.py'
# Author: jcjuarez
# Description:
#      Lazarus Python SDK client implementation.
# ****************************************************

import requests
from typing import Any
from .lazarus_client_error import LazarusClientError

# Python client for the Lazarus Data Store.
class LazarusClient:

    # Value for the Lazarus dedicated success status code.
    SUCCESS_INTERNAL_STATUS_CODE: int = 0
    # Expected internal status code string literal in the server response.
    INTERNAL_STATUS_CODE_RESPONSE_STRING: str = "internal_status_code"
    # Container endpoint path.
    CONTAINER_ENDPOINT: str = "container_endpoint/"
    # Object endpoint path.
    OBJECT_ENDPOINT: str = "object_endpoint/"
    # Ping endpoint path.
    PING_ENDPOINT: str = "ping_endpoint/"
    # Container name string literal for requests and responses.
    CONTAINER_NAME_STRING: str = "container_name"
    # Object ID string literal for requests and responses.
    OBJECT_ID_STRING: str = "object_id"
    # Object data string literal for requests and responses.
    OBJECT_DATA_STRING: str = "object_data"

    # Init method.
    def __init__(
            self,
            host: str,
            port: int,
            timeout: int = 10):
        self.host = host
        self.port = port
        self.base_url = f"http://{host}:{port}/lazarus/network"
        self.timeout = timeout
        self.session = requests.Session()

    # Sends a request to the Lazarus Data Store server.
    # Returns the parsed response object on success, or
    # yields LazarusClientError on failure.
    def _request(
            self,
            method: str,
            endpoint: str,
            **kwargs) -> Any:
        url = f"{self.base_url}/{endpoint}"
        response = None
        try:
            response = self.session.request(method, url, timeout=self.timeout, **kwargs)
            response.raise_for_status()
            # By default, the lazarus server always responds in Json
            # format, so try to convert the response back into such format.
            try:
                response_object = response.json()
                internal_status_code = int(
                    response_object.get(self.INTERNAL_STATUS_CODE_RESPONSE_STRING),
                    base=0)
                if internal_status_code != self.SUCCESS_INTERNAL_STATUS_CODE:
                    # This situation indicates a non-handled HTTP error while
                    # the server still returned a failed internal status code.
                    raise LazarusClientError(
                        response.status_code,
                        internal_status_code,
                        "Lazarus Data Store returned an error.",
                        self.host,
                        self.port) from None
                # If no exception was thrown, and the internal status code
                # is considered as success, return back the parsed response object.
                return response_object
            except Exception:
                # This situation is not expected since the server should
                # always reply back with a well-formed response.
                raise LazarusClientError(
                    None,
                    None,
                    f"Failed to parse the response from Lazarus Data Store. Response={response.text}.",
                    self.host,
                    self.port) from None
        except requests.exceptions.HTTPError:
            try:
                # This indicates an explicit failure coming from the server.
                response_object = response.json()
                internal_status_code = int(
                    response_object.get(self.INTERNAL_STATUS_CODE_RESPONSE_STRING),
                    base=0)
                raise LazarusClientError(
                    response.status_code,
                    internal_status_code,
                    "Lazarus Data Store returned an error.",
                    self.host,
                    self.port) from None
            except Exception:
                # This situation is not expected since the server should
                # always reply back with a well-formed response.
                raise LazarusClientError(
                    None,
                    None,
                    f"Failed to parse the response from Lazarus Data Store. Response={response.text}.",
                    self.host,
                    self.port) from None
        except requests.exceptions.RequestException:
            # This indicates a failure with the request processing.
            raise LazarusClientError(
                None,
                None,
                "Failed to send the request to Lazarus Data Store.",
                self.host,
                self.port) from None

    # ------------------------
    # Container operations.
    # ------------------------
    # Executes a create operation for a container.
    # Yields LazarusClientError on failure.
    def create_container(
            self,
            container_name: str) -> None:
        self._request(
            "PUT",
            self.CONTAINER_ENDPOINT,
            json={self.CONTAINER_NAME_STRING: container_name})

    # Executes a remove operation for a container.
    # Yields LazarusClientError on failure.
    def remove_container(
            self,
            container_name: str) -> None:
        self._request(
            "DELETE",
            self.CONTAINER_ENDPOINT,
            json={self.CONTAINER_NAME_STRING: container_name})

    # ------------------------
    # Object operations.
    # ------------------------
    # Executes an insert operation for an object.
    # Yields LazarusClientError on failure.
    def insert_object(
            self, container_name: str,
            object_id: str,
            data: str) -> None:
        payload = {
            self.CONTAINER_NAME_STRING: container_name,
            self.OBJECT_ID_STRING: object_id,
            self.OBJECT_DATA_STRING: data
        }
        self._request(
            "POST",
            self.OBJECT_ENDPOINT,
            json=payload)

    # Executes a remove operation for an object.
    # Yields LazarusClientError on failure.
    def remove_object(
            self,
            container_name: str,
            object_id: str) -> None:
        payload = {
            self.CONTAINER_NAME_STRING: container_name,
            self.OBJECT_ID_STRING: object_id,
        }
        return self._request(
            "DELETE",
            self.OBJECT_ENDPOINT,
            json=payload)

    # Executes a get operation for an object.
    # Returns the value for the object on success,
    # or yields LazarusClientError on failure.
    def get_object(
            self,
            container_name: str,
            object_id: str) -> str:
        payload = {
            self.CONTAINER_NAME_STRING: container_name,
            self.OBJECT_ID_STRING: object_id,
        }
        result = self._request(
            "GET",
            self.OBJECT_ENDPOINT,
            json=payload)
        # Expect a Json response with an "object_data" field
        # which corresponds to the response for the operation.
        if not isinstance(result, dict) or self.OBJECT_DATA_STRING not in result:
            raise LazarusClientError(
                None,
                None,
                f"Response from Lazarus Data Store is missing the '{self.OBJECT_DATA_STRING}' field.",
                self.host,
                self.port) from None
        return result[self.OBJECT_DATA_STRING]

    # ------------------------
    # Service operations.
    # ------------------------
    # Executes a simple ping service request.
    # Returns True if the ping is successful, False otherwise.
    def ping(self) -> bool:
        try:
            self._request(
                "GET",
                self.PING_ENDPOINT)
            return True
        except LazarusClientError:
            return False
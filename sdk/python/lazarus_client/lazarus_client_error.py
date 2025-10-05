# ****************************************************
# Lazarus Data Store
# SDK
# 'lazarus_client_error.py'
# Author: jcjuarez
# Description:
#      Lazarus Python SDK client error exception.
# ****************************************************

from typing import Optional

# Represents an error from the Lazarus SDK.
class LazarusClientError(Exception):

    # Init method.
    def __init__(
            self,
            http_status_code: Optional[int],
            internal_status_code: Optional[int],
            details: str,
            host: str,
            port: int):
        self.http_status_code = http_status_code
        self.internal_status_code = internal_status_code
        self.details = details
        self.host = host
        self.port = port

    # String method.
    def __str__(
            self) -> str:
        parts = ["LazarusClientError"]
        if self.http_status_code is not None:
            parts.append(f"HTTP={self.http_status_code}")
        if self.internal_status_code is not None:
            parts.append(f"Lazarus=0x{self.internal_status_code:X}")
        parts.append(f"Details={self.details}")
        parts.append(f"Server={self.host}:{self.port}")
        return " ".join(parts)
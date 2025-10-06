# ****************************************************
# Lazarus Data Store
# SDK
# 'lazarus_client_error.py'
# Author: jcjuarez
# Description:
#      Lazarus Python SDK client error exception.
# ****************************************************

from typing import Optional
from .lazarus_status_code import LazarusStatusCode

class LazarusClientError(Exception):

    def __init__(
            self,
            http_status_code: Optional[int],
            lazarus_status_code: Optional[LazarusStatusCode],
            details: str,
            host: str,
            port: int):
        self.http_status_code = http_status_code
        if isinstance(lazarus_status_code, int):
            try:
                self.lazarus_status_code = LazarusStatusCode(lazarus_status_code)
            except ValueError:
                self.lazarus_status_code = lazarus_status_code
        else:
            self.lazarus_status_code = lazarus_status_code
        self.details = details
        self.host = host
        self.port = port

    def __str__(
            self) -> str:
        parts = ["LazarusClientError"]
        if self.http_status_code is not None:
            parts.append(f"HttpStatusCode={self.http_status_code}")
        if self.lazarus_status_code is not None:
            if isinstance(self.lazarus_status_code, LazarusStatusCode):
                parts.append(
                    f"InternalStatusCode={self.lazarus_status_code.name}"
                    f"(0x{self.lazarus_status_code.value:X})"
                )
            else:
                parts.append(f"InternalStatusCode=0x{self.lazarus_status_code:X}")
        parts.append(f"Details={self.details}")
        parts.append(f"Server={self.host}:{self.port}")
        return " ".join(parts)
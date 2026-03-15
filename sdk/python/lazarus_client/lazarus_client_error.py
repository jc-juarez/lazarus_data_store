# ****************************************************
# PandoraDB
# SDK
# 'pandora_client_error.py'
# Author: jcjuarez
# Description:
#      PandoraDB Python SDK client error exception.
# ****************************************************

from typing import Optional
from .pandora_status_code import PandoraDBStatusCode

class PandoraDBClientError(Exception):

    def __init__(
            self,
            http_status_code: Optional[int],
            pandora_status_code: Optional[PandoraDBStatusCode],
            details: str,
            host: str,
            port: int):
        self.http_status_code = http_status_code
        if isinstance(pandora_status_code, int):
            try:
                self.pandora_status_code = PandoraDBStatusCode(pandora_status_code)
            except ValueError:
                self.pandora_status_code = pandora_status_code
        else:
            self.pandora_status_code = pandora_status_code
        self.details = details
        self.host = host
        self.port = port

    def __str__(
            self) -> str:
        parts = ["PandoraDBClientError"]
        if self.http_status_code is not None:
            parts.append(f"HttpStatusCode={self.http_status_code}")
        if self.pandora_status_code is not None:
            if isinstance(self.pandora_status_code, PandoraDBStatusCode):
                parts.append(
                    f"InternalStatusCode={self.pandora_status_code.name}"
                    f"(0x{self.pandora_status_code.value:X})"
                )
            else:
                parts.append(f"InternalStatusCode=0x{self.pandora_status_code:X}")
        parts.append(f"Details={self.details}")
        parts.append(f"Server={self.host}:{self.port}")
        return " ".join(parts)
"""Stub file for the '_hotshot' module."""
# This is an autogenerated file. It serves as a starting point
# for a more precise manual annotation of this module.
# Feel free to edit the source below, but remove this header when you do.

from typing import Any, List, Tuple, Dict, Generic

def coverage(a: str) -> Any: ...
def logreader(a: str) -> LogReaderType: ...
def profiler(a: str, *args, **kwargs) -> Any: ...
def resolution() -> Tuple[Any, ...]: ...

class LogReaderType(object):
    def close(self) -> None: ...
    def fileno(self) -> int: ...

class ProfilerType(object):
    def addinfo(self, a: str, b: str) -> None: ...
    def close(self) -> None: ...
    def fileno(self) -> int: ...
    def runcall(self, *args, **kwargs) -> Any: ...
    def runcode(self, a, b, *args, **kwargs) -> Any: ...
    def start(self) -> None: ...
    def stop(self) -> None: ...
"""Generate include/build_info.h with git tag or branch-shortsha."""

import subprocess
from pathlib import Path

Import("env")  # type: ignore[name-defined]  # noqa: F821


def git_output(args):
    try:
        return subprocess.check_output(
            ["git"] + args,
            text=True,
            stderr=subprocess.DEVNULL,
            cwd=env["PROJECT_DIR"],
        ).strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return ""


def build_version():
    tag = git_output(["describe", "--tags", "--exact-match", "HEAD"])
    if tag:
        return tag

    branch = git_output(["rev-parse", "--abbrev-ref", "HEAD"]) or "unknown"
    commit = git_output(["rev-parse", "--short=7", "HEAD"]) or "0000000"
    if branch == "HEAD":
        branch = "detached"
    branch = branch.replace("/", "-")
    if len(branch) > 24:
        branch = branch[:24]
    return f"{branch}-{commit}"


version = build_version().replace("\\", "\\\\").replace('"', '\\"')
header_path = Path(env["PROJECT_DIR"]) / "include" / "build_info.h"
header_path.write_text(
    "#pragma once\n\n"
    f'#define BUILD_VERSION "{version}"\n',
    encoding="utf-8",
)
print(f"gen_build_info: {version}")

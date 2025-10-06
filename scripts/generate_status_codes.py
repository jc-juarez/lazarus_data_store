# ****************************************************
# Lazarus Data Store
# SDK
# 'generate_status_code.py'
# Author: jcjuarez
# Description:
#      Generates the status codes to be used by
#      the core server and SDK clients.
#      Usage:
#	 * Adding a new status code:
#       python3 generate_status_codes.py \
#          --add <internal_status_code_name> \
#          --http <http_status_code> \
#          --desc "<description>"
#	 * Generate the status files:
# 	    python3 generate_status_codes.py \
#          --generate
# ****************************************************

import sys
import yaml
import pathlib
import argparse

ROOT = pathlib.Path(__file__).resolve().parent.parent
CONFIG_FILE = ROOT / "status_codes.yaml"
SERVER_CPP_FILE = ROOT / "src/status/status.hh"
SDK_PY_FILE = ROOT / "sdk/python/lazarus_client/status.py"

def to_int(val):
    if isinstance(val, int):
        return val
    if isinstance(val, str):
        v = val.replace("'", "").replace("0x", "")
        return int(v, 16)
    raise TypeError(f"Invalid internal code type: {type(val)}")

def to_hex_str(val):
    if isinstance(val, int):
        return f"0x{val:08x}"
    if isinstance(val, str):
        v = val.replace("'", "").replace("0x", "")
        return f"0x{int(v, 16):08x}"
    raise TypeError(f"Invalid internal code type: {type(val)}")

def load_codes():
    if not CONFIG_FILE.exists():
        print(f"[!] Missing {CONFIG_FILE}")
        sys.exit(1)
    data = yaml.safe_load(CONFIG_FILE.read_text())
    return data.get("codes", [])

def save_codes(codes):
    existing = CONFIG_FILE.read_text().rstrip()
    if not existing.endswith("\n"):
        existing += "\n"
    new_code = codes[-1]
    internal_str = to_hex_str(new_code["internal"])
    snippet = (
        "\n"
        f"  - name: {new_code['name']}\n"
        f"    internal: \"{internal_str}\"\n"
        f"    http: {new_code['http']}\n"
        f"    desc: {new_code['desc']}"
    )
    CONFIG_FILE.write_text(existing + snippet)

def next_internal_code(codes):
    max_val = 0
    for c in codes:
        try:
            max_val = max(max_val, to_int(c["internal"]))
        except Exception:
            pass
    return f"0x{max_val + 1:08x}"

def cpp_header():
    return (
        "// ****************************************************\n"
        "// Lazarus Data Store\n"
        "// Status\n"
        "// 'status.hh'\n"
        "// Author: Auto-Generated\n"
        "// Description:\n"
        "//      Status codes for error handling.\n"
        "//      Do not edit manually. Use\n"
        "//      'scripts/generate_status_codes.py' to add\n"
        "//      new status codes.\n"
        "// ****************************************************\n\n"
    )

def py_header():
    return (
        "# ****************************************************\n"
        "# Lazarus Data Store\n"
        "# Status\n"
        "# 'status.py'\n"
        "# Author: Auto-Generated\n"
        "# Description:\n"
        "#      Status codes for error handling.\n"
        "#      Do not edit manually. Use\n"
        "#      'scripts/generate_status_codes.py' to add\n"
        "#      new status codes.\n"
        "# ****************************************************\n\n"
    )

def generate_server_cpp(codes):
    lines = [
        cpp_header(),
        "#pragma once\n\n",
        '#include "status_code.hh"\n',
        "#include <drogon/drogon.h>\n\n",
        "namespace lazarus\n",
        "{\n",
        "namespace status\n",
        "{\n\n",
        "#define status_code_definition(name, internal, http) \\\n"
        "    static inline constexpr status_code name{internal, http, #name}\n\n",
    ]
    for c in codes:
        code_str = to_hex_str(c["internal"])
        http_num = c.get("http", 0)
        desc = c["desc"]
        lines.append(f"// {desc}\n")
        lines.append(
            "status_code_definition(\n"
            f"    {c['name']},\n"
            f"    {code_str},\n"
            f"    static_cast<drogon::HttpStatusCode>({http_num}));\n\n"
        )
        if c["name"] == "fail":
            lines.append(
                "//\n"
                "// Determines whether a given status is considered as failure.\n"
                "//\n"
                "inline static\n"
                "bool\n"
                "failed(\n"
                "    const status_code& status_code)\n"
                "{\n"
                "    return static_cast<std::int32_t>(status_code) < 0;\n"
                "}\n\n"
                "//\n"
                "// Determines whether a given status is considered as success.\n"
                "//\n"
                "inline static\n"
                "bool\n"
                "succeeded(\n"
                "    const status_code& status_code)\n"
                "{\n"
                "    return status_code == success;\n"
                "}\n\n"
            )
    lines.append("} // namespace status.\n")
    lines.append("} // namespace lazarus.")
    SERVER_CPP_FILE.parent.mkdir(parents=True, exist_ok=True)
    text = "".join(lines).rstrip("\n")
    SERVER_CPP_FILE.write_text(text)
    print(f"[✓] Regenerated {SERVER_CPP_FILE}")

def generate_sdk_python(codes):
    lines = [
        py_header(),
        "from enum import Enum\n\n",
        "class LazarusStatus(Enum):\n",
        "\n",
    ]
    for i, c in enumerate(codes):
        val = to_int(c["internal"])
        desc = c.get("desc", "").replace("\n", " ")
        lines.append(f"    # {desc}\n")
        lines.append(f"    {c['name']} = 0x{val:08x}\n")
        if i != len(codes) - 1:
            lines.append("\n")
    lines.append(
        "\n    @classmethod\n"
        "    def from_code(cls, code: int):\n"
        "        for s in cls:\n"
        "            if s.value == code:\n"
        "                return s\n"
        "        return None"
    )
    SDK_PY_FILE.parent.mkdir(parents=True, exist_ok=True)
    text = "".join(lines).rstrip("\n")
    SDK_PY_FILE.write_text(text)
    print(f"[✓] Regenerated {SDK_PY_FILE}")

def add_new_status(name, http, desc):
    codes = load_codes()
    if any(c["name"] == name for c in codes):
        print(f"[!] Status code '{name}' already exists. Aborting.")
        sys.exit(1)
    new_code = {
        "name": name,
        "internal": next_internal_code(codes),
        "http": http,
        "desc": desc,
    }
    codes.append(new_code)
    save_codes(codes)
    print(f"[+] Added new status code: {name}")
    print(f"    Internal code: {new_code['internal']}")
    generate_server_cpp(codes)
    generate_sdk_python(codes)

def regenerate_all():
    codes = load_codes()
    generate_server_cpp(codes)
    generate_sdk_python(codes)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Lazarus Data Store — Status Code Generator",
        epilog=(
            "Usage examples:\n"
            "  Add new code:    python3 scripts/generate_status_codes.py --add <name> --http <code> --desc <description>\n"
            "  Regenerate all:  python3 scripts/generate_status_codes.py --generate"
        ),
        formatter_class=argparse.RawTextHelpFormatter,
    )
    parser.add_argument("--add", help="Add a new status code name (e.g. object_data_empty)")
    parser.add_argument("--http", type=int, help="Associated HTTP code (e.g. 400)")
    parser.add_argument("--desc", help="Description text for the code")
    parser.add_argument("--generate", action="store_true", help="Regenerate all output files")
    args = parser.parse_args()

    # Make sure that no incompatible arguments are provided.
    if args.generate:
        if args.add or args.http or args.desc:
            print("[!] --generate cannot be combined with --add, --http, or --desc.")
            sys.exit(1)
        regenerate_all()
    elif args.add:
        if args.http is None or args.desc is None:
            print("[!] --add requires both --http and --desc parameters.")
            sys.exit(1)
        add_new_status(args.add, args.http, args.desc)
    else:
        print("[!] Must specify either --generate OR --add (with --http and --desc).")
        sys.exit(1)
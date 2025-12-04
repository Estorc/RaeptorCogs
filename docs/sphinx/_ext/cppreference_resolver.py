import requests
from bs4 import BeautifulSoup
from docutils import nodes
from sphinx.domains.cpp import CPPDomain
import re

CPPREF_URL = "https://en.cppreference.com/w/cpp/symbol_index.html"
CPPREF_BASE = "https://en.cppreference.com/w/cpp/"
_cppref_map = None

# Regex for "std::something"
STD_REGEX = re.compile(r"^[A-Za-z0-9_]+")


def build_cppreference_index():
    """
    Download and parse cppreference symbol_index.html.
    Returns a dict mapping 'std::xxx' → URL.
    """
    global _cppref_map
    if _cppref_map is not None:
        return _cppref_map

    print("[cppref] downloading:", CPPREF_URL)
    response = requests.get(CPPREF_URL, timeout=10)

    if response.status_code != 200:
        print("[cppref] ERROR: unable to download cppreference index")
        _cppref_map = {}
        return _cppref_map

    soup = BeautifulSoup(response.text, "html.parser")
    mapping = {}

    for a in soup.select("div#mw-content-text p a"):
        tt = a.find("tt")
        if tt is None:
            continue

        # Extract raw text (may contain &lt; &gt; etc.)
        raw = tt.get_text(strip=True)

        # Accept only C++ identifier-like texts
        identifier = STD_REGEX.match(raw)

        if not identifier:
            continue

        href = a.get("href")
        if not href:
            continue

        mapping[identifier.group(0)] = CPPREF_BASE + href

    print(f"[cppref] loaded {len(mapping)} std:: symbols")
    _cppref_map = mapping

    return mapping


def resolve_std_reference(app, env, node, contnode):
    """
    Called by Sphinx when a reference cannot be resolved.
    If the reference looks like std::xxxx → link to cppreference.
    """
    target = node.get("reftarget")
    if not target:
        return None

    mapping = build_cppreference_index()

    identifier = target.split("<")[0]
    identifier = identifier.split("std::")[-1]
    identifier = identifier.split("::")[0]
    if identifier in mapping:
        return nodes.reference(
            text=identifier, refuri=mapping[identifier], internal=False
        )

    return None


_original_resolve_xref = CPPDomain.resolve_xref
def resolve_xref_fail_std(self, env, fromdocname, builder,
                          typ, target, node, contnode):

    # If it's a standard library symbol → force failure.
    if target.startswith("std::"):
        return None  # make Sphinx emit missing-reference

    # Otherwise, normal behavior
    return _original_resolve_xref(self, env, fromdocname, builder,
                                  typ, target, node, contnode)


def setup(app):
    print("[cppref] setting up cppreference resolver")
    CPPDomain.resolve_xref = resolve_xref_fail_std
    app.connect("missing-reference", resolve_std_reference)
from docutils import nodes


unresolved_references = set()
def log_unresolved_references(app, env, node, contnode):
    target = node.get('reftarget')
    if not target:
        return None
    unresolved_references.add(target)
    return None

def on_build_finished(app, exception):
    if exception is None:
        if unresolved_references:
            print("[logunref] Unresolved references:")
            for ref in sorted(unresolved_references):
                print(f"  - {ref}")

def setup(app):
    print("[logunref] setting up unresolved references logger")
    app.connect("missing-reference", log_unresolved_references, priority=0)
    app.connect("build-finished", on_build_finished)
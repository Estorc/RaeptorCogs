from docutils import nodes


def resolve_glm_reference(app, env, node, contnode):
    target = node.get('reftarget')
    if not target:
        return None

    # GLM matcher
    if target.startswith("glm::"):
        name = target.split("::")[-1]
        url = "https://cglm.readthedocs.io/en/latest/" + name + ".html"
        return nodes.reference(name, name, internal=False, refuri=url)

    return None

def setup(app):
    print("[glmref] setting up glmreference resolver")
    app.connect("missing-reference", resolve_glm_reference)
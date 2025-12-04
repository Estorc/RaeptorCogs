document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll(".toctree-wrapper li").forEach(li => {
        const childList = li.querySelector(":scope > ul");
        if (!childList) return;

        li.classList.add("has-children");

        const link = li.querySelector(":scope > a");

        // create caret
        const caret = document.createElement("span");
        caret.classList.add("caret");
        link.prepend(caret);

        // toggle expand/collapse
        link.addEventListener("click", (event) => {
            event.preventDefault(); // don't navigate on caret click

            if (childList.style.display === "none") {
                childList.style.display = "block";
                caret.classList.add("expanded");
            } else {
                childList.style.display = "none";
                caret.classList.remove("expanded");
            }
        });
    });
});

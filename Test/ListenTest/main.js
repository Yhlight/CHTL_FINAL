(function() {
    if (!window.__chtl_delegate_registry) {
        window.__chtl_delegate_registry = {};
    }
})();


document.querySelector('#my-button').addEventListener('click', () => {
                    document.querySelector('#my-button').textContent = "Clicked!";
                });
document.querySelector('#my-button').addEventListener('mouseenter', function() {
                    console.log("Mouse entered!");
                });
;

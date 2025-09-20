
    console.log("Hello from JavaScript!");

    function greet() {
        const element = document.getElementById('main-content');
        if (element) {
            element.innerText = 'Hello from JavaScript!';
        }
    }

    greet();

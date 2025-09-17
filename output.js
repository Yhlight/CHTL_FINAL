
            animate {
                target: document.querySelector('animated-box'),
                duration: 2000,
                easing: "ease-in-out",
                loop: 2,
                direction: "alternate",

                begin: {
                    background-color: "blue",
                    transform: "scale(1)"
                },

                end: {
                    background-color: "red",
                    transform: "scale(1.5)"
                }
            }

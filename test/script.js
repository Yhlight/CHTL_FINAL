// Main JavaScript for CHTL test
document.addEventListener('DOMContentLoaded', function() {
    console.log('CHTL Test Site loaded');
    
    // Add click handlers to buttons
    const buttons = document.querySelectorAll('.btn');
    buttons.forEach(button => {
        button.addEventListener('click', function() {
            alert('Button clicked!');
        });
    });
    
    // Add smooth scrolling for navigation links
    const navLinks = document.querySelectorAll('.nav-menu a[href^="#"]');
    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            const targetId = this.getAttribute('href').substring(1);
            const targetElement = document.getElementById(targetId);
            if (targetElement) {
                targetElement.scrollIntoView({
                    behavior: 'smooth'
                });
            }
        });
    });
});
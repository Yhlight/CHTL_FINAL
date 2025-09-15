// CHTL JS Basic Test

// CJMOD: printMylove
function printMylove() {
    console.log("I love CHTL!");
}

// CJMOD: iNeverAway
function iNeverAway() {
    return "I will never be away from you!";
}

// 变量声明
{{userName}} = "Chtholly"
{{userAge}} = 17
{{isLoggedIn}} = true

// 选择器使用
const button = document.querySelector('.my-button')
const container = document.querySelector('#main-container')
const items = document.querySelector('.item')

// 事件监听器
button.addEventListener('click', handleClick)
container.addEventListener('mouseover', handleMouseOver)
items.addEventListener('click', handleItemClick)

// 响应式变量
console.log("Hello, {{userName}}!")
console.log("Age: {{userAge}}")
console.log("Logged in: {{isLoggedIn}}")
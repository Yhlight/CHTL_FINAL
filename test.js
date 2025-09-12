// 测试JavaScript文件
console.log('CHTL Import System Test');

function initPage() {
    console.log('Page initialized');
    
    // 添加一些交互功能
    document.addEventListener('DOMContentLoaded', function() {
        console.log('DOM loaded');
        
        // 查找所有按钮并添加点击事件
        const buttons = document.querySelectorAll('button');
        buttons.forEach(button => {
            button.addEventListener('click', function() {
                console.log('Button clicked:', this.textContent);
            });
        });
    });
}

// 调用初始化函数
initPage();
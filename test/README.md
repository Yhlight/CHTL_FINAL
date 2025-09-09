# CHTL 测试文件

这个目录包含 CHTL 编译器的测试文件。

## 测试文件说明

### simple.chtl
一个简单的 CHTL 测试文件，包含：
- HTML 结构（html, head, body, div）
- 文本节点
- 属性设置（id, class）
- 局部样式块

## 编译测试

```bash
# 编译测试文件
./bin/chtl test/simple.chtl -o test/output

# 查看生成的 HTML
cat test/output.html

# 查看生成的 CSS
cat test/output.css

# 查看生成的 JavaScript
cat test/output.js
```

## 预期输出

### HTML (test/output.html)
```html
<html>
  <head>
    <title>
      CHTL Test Page
    </title>
  </head>
  <body>
    <div id="container" class="main-content">
      Hello, CHTL World!
    </div>
    <div>
      <style>
        width: 100px;
        height: 100px;
        background-color: red;
      </style>
    </div>
  </body>
</html>
```

### CSS (test/output.css)
```css
body {
  width: 100px;
  height: 100px;
  background-color: red;
}
```

### JavaScript (test/output.js)
```javascript
// 暂无内容
```
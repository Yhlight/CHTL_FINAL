// 外部 JavaScript 文件 - 用于测试导入功能

// 工具函数
const Utils = {
    // 防抖函数
    debounce: function(func, wait) {
        let timeout;
        return function executedFunction(...args) {
            const later = () => {
                clearTimeout(timeout);
                func(...args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    },
    
    // 节流函数
    throttle: function(func, limit) {
        let inThrottle;
        return function() {
            const args = arguments;
            const context = this;
            if (!inThrottle) {
                func.apply(context, args);
                inThrottle = true;
                setTimeout(() => inThrottle = false, limit);
            }
        };
    },
    
    // 生成随机 ID
    generateId: function() {
        return Math.random().toString(36).substr(2, 9);
    },
    
    // 格式化日期
    formatDate: function(date) {
        return new Intl.DateTimeFormat('zh-CN', {
            year: 'numeric',
            month: '2-digit',
            day: '2-digit',
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
        }).format(date);
    },
    
    // 深拷贝
    deepClone: function(obj) {
        if (obj === null || typeof obj !== 'object') return obj;
        if (obj instanceof Date) return new Date(obj.getTime());
        if (obj instanceof Array) return obj.map(item => this.deepClone(item));
        if (typeof obj === 'object') {
            const clonedObj = {};
            for (const key in obj) {
                if (obj.hasOwnProperty(key)) {
                    clonedObj[key] = this.deepClone(obj[key]);
                }
            }
            return clonedObj;
        }
    }
};

// 事件管理器
const EventManager = {
    events: {},
    
    // 订阅事件
    on: function(event, callback) {
        if (!this.events[event]) {
            this.events[event] = [];
        }
        this.events[event].push(callback);
    },
    
    // 取消订阅
    off: function(event, callback) {
        if (this.events[event]) {
            this.events[event] = this.events[event].filter(cb => cb !== callback);
        }
    },
    
    // 触发事件
    emit: function(event, data) {
        if (this.events[event]) {
            this.events[event].forEach(callback => callback(data));
        }
    }
};

// 动画管理器
const AnimationManager = {
    // 淡入动画
    fadeIn: function(element, duration = 300) {
        element.style.opacity = '0';
        element.style.display = 'block';
        
        let start = performance.now();
        
        function animate(currentTime) {
            const elapsed = currentTime - start;
            const progress = Math.min(elapsed / duration, 1);
            
            element.style.opacity = progress;
            
            if (progress < 1) {
                requestAnimationFrame(animate);
            }
        }
        
        requestAnimationFrame(animate);
    },
    
    // 淡出动画
    fadeOut: function(element, duration = 300) {
        let start = performance.now();
        const initialOpacity = parseFloat(getComputedStyle(element).opacity);
        
        function animate(currentTime) {
            const elapsed = currentTime - start;
            const progress = Math.min(elapsed / duration, 1);
            
            element.style.opacity = initialOpacity * (1 - progress);
            
            if (progress < 1) {
                requestAnimationFrame(animate);
            } else {
                element.style.display = 'none';
            }
        }
        
        requestAnimationFrame(animate);
    },
    
    // 滑动动画
    slideDown: function(element, duration = 300) {
        element.style.height = '0';
        element.style.overflow = 'hidden';
        element.style.display = 'block';
        
        const targetHeight = element.scrollHeight;
        let start = performance.now();
        
        function animate(currentTime) {
            const elapsed = currentTime - start;
            const progress = Math.min(elapsed / duration, 1);
            
            element.style.height = (targetHeight * progress) + 'px';
            
            if (progress < 1) {
                requestAnimationFrame(animate);
            } else {
                element.style.height = 'auto';
                element.style.overflow = 'visible';
            }
        }
        
        requestAnimationFrame(animate);
    }
};

// 本地存储管理器
const StorageManager = {
    // 设置存储
    set: function(key, value) {
        try {
            localStorage.setItem(key, JSON.stringify(value));
            return true;
        } catch (e) {
            console.error('存储失败:', e);
            return false;
        }
    },
    
    // 获取存储
    get: function(key) {
        try {
            const item = localStorage.getItem(key);
            return item ? JSON.parse(item) : null;
        } catch (e) {
            console.error('读取存储失败:', e);
            return null;
        }
    },
    
    // 删除存储
    remove: function(key) {
        try {
            localStorage.removeItem(key);
            return true;
        } catch (e) {
            console.error('删除存储失败:', e);
            return false;
        }
    },
    
    // 清空存储
    clear: function() {
        try {
            localStorage.clear();
            return true;
        } catch (e) {
            console.error('清空存储失败:', e);
            return false;
        }
    }
};

// 网络请求管理器
const NetworkManager = {
    // GET 请求
    get: function(url, options = {}) {
        return this.request('GET', url, null, options);
    },
    
    // POST 请求
    post: function(url, data, options = {}) {
        return this.request('POST', url, data, options);
    },
    
    // 通用请求方法
    request: function(method, url, data, options = {}) {
        return new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            
            xhr.open(method, url);
            
            // 设置请求头
            if (options.headers) {
                Object.keys(options.headers).forEach(key => {
                    xhr.setRequestHeader(key, options.headers[key]);
                });
            }
            
            xhr.onload = function() {
                if (xhr.status >= 200 && xhr.status < 300) {
                    try {
                        const response = JSON.parse(xhr.responseText);
                        resolve(response);
                    } catch (e) {
                        resolve(xhr.responseText);
                    }
                } else {
                    reject(new Error(`HTTP ${xhr.status}: ${xhr.statusText}`));
                }
            };
            
            xhr.onerror = function() {
                reject(new Error('网络请求失败'));
            };
            
            if (data) {
                xhr.send(JSON.stringify(data));
            } else {
                xhr.send();
            }
        });
    }
};

// 初始化函数
function initializeExternalScript() {
    console.log('外部 JavaScript 脚本已加载');
    console.log('可用工具:', { Utils, EventManager, AnimationManager, StorageManager, NetworkManager });
    
    // 全局暴露工具
    window.ExternalUtils = {
        Utils,
        EventManager,
        AnimationManager,
        StorageManager,
        NetworkManager
    };
}

// 页面加载完成后初始化
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initializeExternalScript);
} else {
    initializeExternalScript();
}
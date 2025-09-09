package context

import (
	"fmt"
	"path/filepath"
	"sync"
)

// Context 编译上下文，管理编译过程中的全局状态
type Context struct {
	debugMode     bool
	workingDir    string
	outputDir     string
	modules       map[string]interface{}
	namespaces    map[string]*Namespace
	config        *Configuration
	mu            sync.RWMutex
}

// Namespace 命名空间
type Namespace struct {
	Name      string
	Elements  map[string]interface{}
	Styles    map[string]interface{}
	Variables map[string]interface{}
	Parent    *Namespace
}

// Configuration 配置信息
type Configuration struct {
	IndexInitialCount        int
	DebugMode               bool
	DisableStyleAutoAddClass bool
	DisableStyleAutoAddID   bool
	DisableDefaultNamespace bool
	DisableCustomOriginType bool
	DisableScriptAutoAddClass bool
	DisableScriptAutoAddID  bool
	Names                   map[string]interface{}
	OriginTypes             map[string]string
}

// NewContext 创建新的编译上下文
func NewContext() *Context {
	return &Context{
		debugMode:  false,
		modules:    make(map[string]interface{}),
		namespaces: make(map[string]*Namespace),
		config:     NewDefaultConfiguration(),
	}
}

// NewDefaultConfiguration 创建默认配置
func NewDefaultConfiguration() *Configuration {
	return &Configuration{
		IndexInitialCount:        0,
		DebugMode:               false,
		DisableStyleAutoAddClass: false,
		DisableStyleAutoAddID:   false,
		DisableDefaultNamespace: false,
		DisableCustomOriginType: false,
		DisableScriptAutoAddClass: true,
		DisableScriptAutoAddID:  true,
		Names:                   make(map[string]interface{}),
		OriginTypes:             make(map[string]string),
	}
}

// SetDebugMode 设置调试模式
func (c *Context) SetDebugMode(debug bool) {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.debugMode = debug
	c.config.DebugMode = debug
}

// IsDebugMode 检查是否为调试模式
func (c *Context) IsDebugMode() bool {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.debugMode
}

// SetWorkingDir 设置工作目录
func (c *Context) SetWorkingDir(dir string) {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.workingDir = dir
}

// GetWorkingDir 获取工作目录
func (c *Context) GetWorkingDir() string {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.workingDir
}

// SetOutputDir 设置输出目录
func (c *Context) SetOutputDir(dir string) {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.outputDir = dir
}

// GetOutputDir 获取输出目录
func (c *Context) GetOutputDir() string {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.outputDir
}

// AddModule 添加模块
func (c *Context) AddModule(name string, module interface{}) {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.modules[name] = module
}

// GetModule 获取模块
func (c *Context) GetModule(name string) (interface{}, bool) {
	c.mu.RLock()
	defer c.mu.RUnlock()
	module, exists := c.modules[name]
	return module, exists
}

// AddNamespace 添加命名空间
func (c *Context) AddNamespace(name string, namespace *Namespace) {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.namespaces[name] = namespace
}

// GetNamespace 获取命名空间
func (c *Context) GetNamespace(name string) (*Namespace, bool) {
	c.mu.RLock()
	defer c.mu.Unlock()
	namespace, exists := c.namespaces[name]
	return namespace, exists
}

// GetConfig 获取配置
func (c *Context) GetConfig() *Configuration {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.config
}

// SetConfig 设置配置
func (c *Context) SetConfig(config *Configuration) {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.config = config
}

// ResolvePath 解析文件路径
func (c *Context) ResolvePath(path string) string {
	if filepath.IsAbs(path) {
		return path
	}
	return filepath.Join(c.GetWorkingDir(), path)
}

// DebugLog 调试日志
func (c *Context) DebugLog(format string, args ...interface{}) {
	if c.IsDebugMode() {
		fmt.Printf("[DEBUG] "+format+"\n", args...)
	}
}

// ErrorLog 错误日志
func (c *Context) ErrorLog(format string, args ...interface{}) {
	fmt.Printf("[ERROR] "+format+"\n", args...)
}

// WarnLog 警告日志
func (c *Context) WarnLog(format string, args ...interface{}) {
	fmt.Printf("[WARN] "+format+"\n", args...)
}
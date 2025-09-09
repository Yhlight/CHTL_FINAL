#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

/**
 * @brief 动画节点
 * 
 * 表示 animate {} 语法
 */
class AnimateNode : public CHTLJSBaseNode {
public:
    struct Keyframe {
        double at;                          // 动画播放时刻
        std::map<std::string, std::string> properties; // CSS属性
    };
    
    AnimateNode(size_t line = 0, size_t column = 0);
    
    // 设置目标选择器
    void setTarget(const std::string& target);
    
    // 获取目标选择器
    const std::string& getTarget() const { return target_; }
    
    // 设置动画持续时间
    void setDuration(int duration);
    
    // 获取动画持续时间
    int getDuration() const { return duration_; }
    
    // 设置缓动函数
    void setEasing(const std::string& easing);
    
    // 获取缓动函数
    const std::string& getEasing() const { return easing_; }
    
    // 设置起始状态
    void setBeginState(const std::map<std::string, std::string>& properties);
    
    // 获取起始状态
    const std::map<std::string, std::string>& getBeginState() const { return beginState_; }
    
    // 添加关键帧
    void addKeyframe(const Keyframe& keyframe);
    
    // 获取所有关键帧
    const std::vector<Keyframe>& getKeyframes() const { return keyframes_; }
    
    // 设置结束状态
    void setEndState(const std::map<std::string, std::string>& properties);
    
    // 获取结束状态
    const std::map<std::string, std::string>& getEndState() const { return endState_; }
    
    // 设置循环次数
    void setLoop(int loop);
    
    // 获取循环次数
    int getLoop() const { return loop_; }
    
    // 设置播放方向
    void setDirection(const std::string& direction);
    
    // 获取播放方向
    const std::string& getDirection() const { return direction_; }
    
    // 设置延迟
    void setDelay(int delay);
    
    // 获取延迟
    int getDelay() const { return delay_; }
    
    // 设置回调函数
    void setCallback(const std::string& callback);
    
    // 获取回调函数
    const std::string& getCallback() const { return callback_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string target_;
    int duration_;
    std::string easing_;
    std::map<std::string, std::string> beginState_;
    std::vector<Keyframe> keyframes_;
    std::map<std::string, std::string> endState_;
    int loop_;
    std::string direction_;
    int delay_;
    std::string callback_;
    
    // 生成CSS属性字符串
    std::string generateCSSProperties(const std::map<std::string, std::string>& properties) const;
};

} // namespace CHTL
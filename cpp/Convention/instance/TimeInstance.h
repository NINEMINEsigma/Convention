#ifndef __FILE_CONVENTION_TIME_INSTANCE
#define __FILE_CONVENTION_TIME_INSTANCE

#include "Convention/instance/Interface.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <vector>

/**
 * @brief 时间单位枚举类
 * 定义了从纳秒到天的不同时间单位
 */
enum class TimeUnit {
    Nanoseconds,    ///< 纳秒
    Microseconds,   ///< 微秒
    Milliseconds,   ///< 毫秒
    Seconds,        ///< 秒
    Minutes,        ///< 分钟
    Hours,          ///< 小时
    Days           ///< 天
};

/**
 * @brief 时间格式化选项结构体
 * 用于控制时间格式化输出的各种选项
 */
struct TimeFormatOptions {
    bool include_date = true;         ///< 是否包含日期部分
    bool include_time = true;         ///< 是否包含时间部分
    bool include_milliseconds = false; ///< 是否包含毫秒
    std::string date_separator = "-";  ///< 日期分隔符
    std::string time_separator = ":";  ///< 时间分隔符
    std::string date_time_separator = " "; ///< 日期和时间之间的分隔符
};

/**
 * @brief 时间实例类
 * 基于高精度时钟的时间点实例,提供时间测量、格式化和计时器功能
 */
template<>
class instance<std::chrono::time_point<std::chrono::high_resolution_clock>, true> 
    :public instance<std::chrono::time_point<std::chrono::high_resolution_clock>, false>
{
public:
    using _Clock = std::chrono::high_resolution_clock;
    using _TimePoint = std::chrono::time_point<_Clock>;
    using _Mybase = instance<_TimePoint, false>;

private:
    _TimePoint m_begin;              ///< 开始时间点
    _TimePoint m_end;                ///< 结束时间点
    std::vector<_TimePoint> m_checkpoints;       ///< 检查点时间列表
    std::vector<std::string> m_checkpoint_labels; ///< 检查点标签列表

public:
    /**
     * @brief 本地化计时器类
     * 提供局部作用域的计时功能,支持暂停/恢复
     */
    class localized_clocker :public any_class
    {
        _TimePoint m_begin;                      ///< 计时器开始时间
        instance<_TimePoint, true>& parent;      ///< 父实例引用
        bool m_is_paused;                        ///< 暂停状态标志
        std::chrono::nanoseconds m_accumulated_time; ///< 累计暂停时间
        _TimePoint m_pause_time;                 ///< 暂停开始时间

    public:
        /**
         * @brief 构造函数
         * @param parent 父时间实例的引用
         */
        localized_clocker(instance<_TimePoint, true>& parent)
            : m_begin(_Clock::now()),
            parent(parent),
            m_is_paused(false),
            m_accumulated_time(std::chrono::nanoseconds(0)) {}

        /**
         * @brief 移动构造函数
         * @param other 要移动的其他计时器实例
         */
        localized_clocker(localized_clocker&& other) noexcept
            :m_begin(_Clock::now()),
            parent(other.parent),
            m_is_paused(other.m_is_paused),
            m_accumulated_time(other.m_accumulated_time),
            m_pause_time(other.m_pause_time) {}

        /**
         * @brief 析构函数
         * 更新父实例的开始和结束时间
         */
        virtual ~localized_clocker()
        {
            parent.m_begin = this->m_begin;
            parent.m_end = _Clock::now();
        }

        /**
         * @brief 重置计时器
         * 重置所有计时相关状态
         */
        void reset() 
        {
            m_begin = _Clock::now();
            m_is_paused = false;
            m_accumulated_time = std::chrono::nanoseconds(0);
        }

        /**
         * @brief 暂停计时器
         */
        void pause()
        {
            if (!m_is_paused) {
                m_pause_time = _Clock::now();
                m_is_paused = true;
            }
        }

        /**
         * @brief 恢复计时器
         */
        void resume()
        {
            if (m_is_paused) {
                m_accumulated_time += std::chrono::duration_cast<std::chrono::nanoseconds>(_Clock::now() - m_pause_time);
                m_is_paused = false;
            }
        }

        /**
         * @brief 获取暂停状态
         * @return 是否处于暂停状态
         */
        bool is_paused() const { return m_is_paused; }

        /**
         * @brief 获取持续时间
         * @tparam _TimeUnit 时间单位类型
         * @return 计时器运行时间(不包含暂停时间)
         */
        template<typename _TimeUnit = std::chrono::milliseconds>
        int64_t duration() const
        {
            auto current = m_is_paused ? m_pause_time : _Clock::now();
            auto total_time = current - m_begin - m_accumulated_time;
            return std::chrono::duration_cast<_TimeUnit>(total_time).count();
        }

        /**
         * @brief 获取持续秒数
         * @return 计时器运行的秒数
         */
        double seconds() const
        {
            return duration<std::chrono::milliseconds>() * 0.001;
        }

        /**
         * @brief 等待直到指定持续时间
         * @tparam _TimeUnit 时间单位类型
         * @param target_duration 目标持续时间
         */
        template<typename _TimeUnit>
        void wait_until(int64_t target_duration)
        {
            if (m_is_paused) return;

            auto current_duration = duration<_TimeUnit>();
            if (current_duration < target_duration) {
                auto sleep_time = target_duration - current_duration;
                std::this_thread::sleep_for(_TimeUnit(sleep_time));
            }
        }
    };

    /**
     * @brief 默认构造函数
     */
    instance() :_Mybase(new _TimePoint(_Clock::now())) {};
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的其他实例
     */
    instance(const instance& other) :_Mybase(other)
    {
        this->m_begin = other.m_begin;
        this->m_end = other.m_end;
        this->m_checkpoints = other.m_checkpoints;
        this->m_checkpoint_labels = other.m_checkpoint_labels;
    }

    // 时间点获取方法
    auto get_create_time() const { return **this; }
    decltype(auto) create_time() const { return **this; }
    auto get_begin_time() const { return this->m_begin; }
    auto get_end_time() const { return this->m_end; }

    /**
     * @brief 计算从创建时间开始的持续时间
     * @tparam _TimeUnit 时间单位类型
     * @return 持续时间值
     */
    template<typename _TimeUnit = std::chrono::milliseconds>
    int64_t from_create_duration() const
    {
        return std::chrono::duration_cast<_TimeUnit>(_Clock::now() - **this).count();
    }

    /**
     * @brief 计算从创建时间开始的持续秒数
     * @return 持续秒数
     */
    double from_create_seconds() const
    {
        return from_create_duration<std::chrono::milliseconds>() * 0.001;
    }

    /**
     * @brief 创建本地化计时器
     * @return 新的本地化计时器实例
     */
    auto make_localized_clocker()
    {
        return localized_clocker(*this);
    }

    /**
     * @brief 获取最后一次本地化计时器的持续时间
     * @tparam _TimeUnit 时间单位类型
     * @return 持续时间值
     */
    template<typename _TimeUnit = std::chrono::milliseconds>
    int64_t last_localized_clocker_duration() const
    {
        return std::chrono::duration_cast<_TimeUnit>(m_end - m_begin).count();
    }

    /**
     * @brief 获取最后一次本地化计时器的持续秒数
     * @return 持续秒数
     */
    double last_localized_clocker_seconds() const
    {
        return last_localized_clocker_duration<std::chrono::milliseconds>() * 0.001;
    }

    /**
     * @brief 添加检查点
     * @param label 检查点标签
     */
    void add_checkpoint(const std::string& label = "")
    {
        m_checkpoints.push_back(_Clock::now());
        m_checkpoint_labels.push_back(label);
    }

    /**
     * @brief 清除所有检查点
     */
    void clear_checkpoints()
    {
        m_checkpoints.clear();
        m_checkpoint_labels.clear();
    }

    /**
     * @brief 获取检查点之间的持续时间
     * @tparam _TimeUnit 时间单位类型
     * @return 持续时间列表
     */
    template<typename _TimeUnit = std::chrono::milliseconds>
    std::vector<int64_t> get_checkpoint_durations() const
    {
        std::vector<int64_t> durations;
        if (m_checkpoints.empty()) return durations;

        for (size_t i = 1; i < m_checkpoints.size(); ++i) {
            durations.push_back(
                std::chrono::duration_cast<_TimeUnit>(
                    m_checkpoints[i] - m_checkpoints[i-1]
                ).count()
            );
        }
        return durations;
    }

    /**
     * @brief 获取检查点摘要
     * @return 检查点标签和持续时间的配对列表
     */
    std::vector<std::pair<std::string, double>> get_checkpoint_summary() const
    {
        std::vector<std::pair<std::string, double>> summary;
        auto durations = get_checkpoint_durations<std::chrono::milliseconds>();
        
        for (size_t i = 0; i < durations.size(); ++i) {
            summary.emplace_back(
                m_checkpoint_labels[i+1],
                durations[i] * 0.001
            );
        }
        return summary;
    }

    /**
     * @brief 格式化时间点
     * @param tp 要格式化的时间点
     * @param options 格式化选项
     * @return 格式化后的字符串
     */
    static std::string format_time_point(const _TimePoint& tp, const TimeFormatOptions& options = TimeFormatOptions())
    {
        using namespace std::chrono;
        auto sys_now = system_clock::now();
        auto high_now = _Clock::now();
        auto sys_tp = sys_now + duration_cast<system_clock::duration>(tp - high_now);
        auto time = system_clock::to_time_t(sys_tp);
        auto ms = duration_cast<milliseconds>(
            tp.time_since_epoch()
        ).count() % 1000;

        std::stringstream ss;
        std::tm tm_time;
#ifdef _WIN32
        localtime_s(&tm_time, &time);
#else
        localtime_r(&time, &tm_time);
#endif

        if (options.include_date) {
            ss << std::setfill('0')
               << std::setw(4) << (tm_time.tm_year + 1900) << options.date_separator
               << std::setw(2) << (tm_time.tm_mon + 1) << options.date_separator
               << std::setw(2) << tm_time.tm_mday;
        }

        if (options.include_date && options.include_time) {
            ss << options.date_time_separator;
        }

        if (options.include_time) {
            ss << std::setfill('0')
               << std::setw(2) << tm_time.tm_hour << options.time_separator
               << std::setw(2) << tm_time.tm_min << options.time_separator
               << std::setw(2) << tm_time.tm_sec;

            if (options.include_milliseconds) {
                ss << "." << std::setw(3) << ms;
            }
        }

        return ss.str();
    }

    /**
     * @brief 格式化创建时间
     * @param options 格式化选项
     * @return 格式化后的字符串
     */
    std::string format_create_time(const TimeFormatOptions& options = TimeFormatOptions()) const
    {
        return format_time_point(**this, options);
    }

    /**
     * @brief 格式化开始时间
     * @param options 格式化选项
     * @return 格式化后的字符串
     */
    std::string format_begin_time(const TimeFormatOptions& options = TimeFormatOptions()) const
    {
        return format_time_point(m_begin, options);
    }

    /**
     * @brief 格式化结束时间
     * @param options 格式化选项
     * @return 格式化后的字符串
     */
    std::string format_end_time(const TimeFormatOptions& options = TimeFormatOptions()) const
    {
        return format_time_point(m_end, options);
    }

    /**
     * @brief 时间单位转换
     * @tparam _TimeUnit 时间单位类型
     * @param value 要转换的值
     * @param from_unit 源时间单位
     * @param to_unit 目标时间单位
     * @return 转换后的值
     */
    template<typename _TimeUnit>
    static int64_t convert_duration(int64_t value, TimeUnit from_unit, TimeUnit to_unit)
    {
        using namespace std::chrono;
        
        int64_t nanoseconds;
        switch (from_unit) {
            case TimeUnit::Nanoseconds:  nanoseconds = value; break;
            case TimeUnit::Microseconds: nanoseconds = value * 1000; break;
            case TimeUnit::Milliseconds: nanoseconds = value * 1000000; break;
            case TimeUnit::Seconds:      nanoseconds = value * 1000000000; break;
            case TimeUnit::Minutes:      nanoseconds = value * 60000000000; break;
            case TimeUnit::Hours:        nanoseconds = value * 3600000000000; break;
            case TimeUnit::Days:         nanoseconds = value * 86400000000000; break;
            default: return 0;
        }

        switch (to_unit) {
            case TimeUnit::Nanoseconds:  return nanoseconds;
            case TimeUnit::Microseconds: return nanoseconds / 1000;
            case TimeUnit::Milliseconds: return nanoseconds / 1000000;
            case TimeUnit::Seconds:      return nanoseconds / 1000000000;
            case TimeUnit::Minutes:      return nanoseconds / 60000000000;
            case TimeUnit::Hours:        return nanoseconds / 3600000000000;
            case TimeUnit::Days:         return nanoseconds / 86400000000000;
            default: return 0;
        }
    }

    /**
     * @brief 线程睡眠
     * @tparam _TimeUnit 时间单位类型
     * @param duration 睡眠时间
     */
    template<typename _TimeUnit>
    static void sleep(int64_t duration)
    {
        std::this_thread::sleep_for(_TimeUnit(duration));
    }

    /**
     * @brief 设置定时器间隔执行
     * @tparam _Func 函数类型
     * @param func 要执行的函数
     * @param interval_ms 间隔毫秒数
     * @param should_continue 控制继续执行的标志
     */
    template<typename _Func>
    static void set_interval(_Func func, int64_t interval_ms, bool& should_continue)
    {
        while (should_continue) {
            auto start = _Clock::now();
            func();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(_Clock::now() - start).count();
            if (elapsed < interval_ms) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms - elapsed));
            }
        }
    }

    /**
     * @brief 设置延时执行
     * @tparam _Func 函数类型
     * @param func 要执行的函数
     * @param timeout_ms 延时毫秒数
     */
    template<typename _Func>
    static void set_timeout(_Func func, int64_t timeout_ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
        func();
    }
};

#endif // !__FILE_CONVENTION_TIME_INSTANCE

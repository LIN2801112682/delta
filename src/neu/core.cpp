#include "neu/core.h"

namespace neu
{
    std::stack<node>
    backtracking_path(std::string_view basic_str, std::string_view log_str)
    {
        std::stack<node> node_stack{};
        int m{static_cast<int>(basic_str.size())};
        int n{static_cast<int>(log_str.size())};
        int dp[m + 1][n + 1];

        for (int i{0}; i < m + 1; ++i)
        {
            dp[i][0] = i;
        }
        for (int i{0}; i < n + 1; ++i)
        {
            dp[0][i] = i;
        }

        for (int i{1}; i < m + 1; ++i)
        {
            for (int j{1}; j < n + 1; ++j)
            {
                if (basic_str[i - 1] == log_str[j - 1])
                {
                    dp[i][j] = dp[i - 1][j - 1];
                }
                else
                {
                    dp[i][j] = std::min(std::min(dp[i - 1][j] + 1, dp[i][j - 1] + 1), dp[i - 1][j - 1] + 1);
                }
            }
        }

#if 0
        std::cout << "distance: " << dp[m][n] << '\n';
#endif

        while (m >= 0 || n >= 0)
        {

            if (n != 0 && dp[m][n - 1] + 1 == dp[m][n])
            {
#if 0
                std::cout << "insert: " << log_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node a{m - 1, m, node_type::insert, std::string{log_str[n - 1]}};
                if (node_stack.empty())
                {
                    node_stack.push(a);
                }
                else
                {
                    node b{node_stack.top()};
                    if (a.type_ == b.type_ && a.low_ == b.low_)
                    {
                        a.content_ += b.content_;
                        node_stack.pop();
                    }
                    node_stack.push(a);
                }
                --n;
                continue;
            }
            else if (m != 0 && dp[m - 1][n] + 1 == dp[m][n])
            {
#if 0
                std::cout << "delete: " << basic_str[m - 1] << " at: " << m - 1 << '\n';
#endif
                node a{m - 1, m - 1, node_type::del, ""};
                if (node_stack.empty())
                {
                    node_stack.push(a);
                }
                else
                {
                    node b{node_stack.top()};
                    if (a.type_ == b.type_ && a.low_ + 1 == b.low_)
                    {
                        a.high_ = b.high_;
                        node_stack.pop();
                    }
                    node_stack.push(a);
                }
                --m;
                continue;
            }
            else if (dp[m - 1][n - 1] + 1 == dp[m][n])
            {
#if 0
                std::cout << "replace: " << basic_str[m - 1] << " to: " << log_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node a{m - 1, m - 1, node_type::replace, std::string{log_str[n - 1]}};
                if (node_stack.empty())
                {
                    node_stack.push(a);
                }
                else
                {
                    node b{node_stack.top()};
                    if (a.type_ == b.type_ && a.low_ + 1 == b.low_)
                    {
                        a.high_ = b.high_;
                        a.content_ += b.content_;
                        node_stack.pop();
                    }
                    node_stack.push(a);
                }
                --m;
                --n;
                continue;
            }
            --m;
            --n;
        }
        return node_stack;
    }

    std::string merge_str_by_node_stack(std::string_view basic_str, std::stack<node> &&node_stack)
    {
        std::string merged_str{};
        int i{0};
        while (i < basic_str.size() || !node_stack.empty())
        {
            if (node_stack.empty())
            {
                merged_str += basic_str.substr(i);
                break;
            }
            auto d{node_stack.top()};
            node_stack.pop();
            switch (d.type_)
            {
            case node_type::insert:
                merged_str += basic_str.substr(i, d.low_ - i + 1);
                merged_str += d.content_;
                i = d.high_;
                break;
            case node_type::del:
                merged_str += basic_str.substr(i, d.low_ - i);
                i = d.high_ + 1;
                break;
            case node_type::replace:
                merged_str += basic_str.substr(i, d.low_ - i);
                merged_str += d.content_;
                i = d.high_ + 1;
                break;
            default:
                assert(false);
                break;
            }
        }
        return merged_str;
    }

    std::string merge_str_by_delta(std::string_view basic_str, const delta_type &delta)
    {
        std::string merged_str{};
        int i{0};
        int j{0};
        while (i < basic_str.size() || j < delta.size())
        {
            if (j == delta.size())
            {
                merged_str += basic_str.substr(i);
                break;
            }
            auto d{delta[j]};
            ++j;
            switch (d.type_)
            {
            case node_type::insert:
                merged_str += basic_str.substr(i, d.low_ - i + 1);
                merged_str += d.content_;
                i = d.high_;
                break;
            case node_type::del:
                merged_str += basic_str.substr(i, d.low_ - i);
                i = d.high_ + 1;
                break;
            case node_type::replace:
                merged_str += basic_str.substr(i, d.low_ - i);
                merged_str += d.content_;
                i = d.high_ + 1;
                break;
            default:
                assert(false);
                break;
            }
        }
        return merged_str;
    }
};
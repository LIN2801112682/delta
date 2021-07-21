#include "neu/core.h"
#include <array>

#define DEBUG_PRINT 0

namespace neu
{
    std::stack<node> backtracking_path(std::string_view basic_str, std::string_view log_str)
    {
        std::stack<node> delta{};
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

#if DEBUG_PRINT
        std::cout << "distance: " << dp[m][n] << '\n';
#endif

        while (m >= 0 || n >= 0)
        {

            if (n != 0 && dp[m][n - 1] + 1 == dp[m][n])
            {
#if DEBUG_PRINT
                std::cout << "insert: " << log_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node a{m - 1, m, node_type::insert, std::string{log_str[n - 1]}};
                if (delta.empty())
                {
                    delta.push(a);
                }
                else
                {
                    node b{delta.top()};
                    if (a.type_ == b.type_ && a.low_ == b.low_)
                    {
                        a.content_ += b.content_;
                        delta.pop();
                    }
                    delta.push(a);
                }
                --n;
                continue;
            }
            else if (m != 0 && dp[m - 1][n] + 1 == dp[m][n])
            {
#if DEBUG_PRINT
                std::cout << "delete: " << basic_str[m - 1] << " at: " << m - 1 << '\n';
#endif
                node a{m - 1, m - 1, node_type::del, "-"};  // todo: tokenlize
                if (delta.empty())
                {
                    delta.push(a);
                }
                else
                {
                    node b{delta.top()};
                    if (a.type_ == b.type_ && a.low_ + 1 == b.low_)
                    {
                        a.high_ = b.high_;
                        delta.pop();
                    }
                    delta.push(a);
                }
                --m;
                continue;
            }
            else if (dp[m - 1][n - 1] + 1 == dp[m][n])
            {
#if DEBUG_PRINT
                std::cout << "replace: " << basic_str[m - 1] << " to: " << log_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node a{m - 1, m - 1, node_type::replace, std::string{log_str[n - 1]}};
                if (delta.empty())
                {
                    delta.push(a);
                }
                else
                {
                    node b{delta.top()};
                    if (a.type_ == b.type_ && a.low_ + 1 == b.low_)
                    {
                        a.high_ = b.high_;
                        a.content_ += b.content_;
                        delta.pop();
                    }
                    delta.push(a);
                }
                --m;
                --n;
                continue;
            }
            --m;
            --n;
        }
        return delta;
    }

    std::string merge_str(std::string_view basic_str, std::stack<node> &&delta)
    {
        std::string merged_str{};
        int i{0};
        while (i < basic_str.size())
        {
            if (delta.empty())
            {
                merged_str += basic_str.substr(i);
                break;
            }
            auto d{delta.top()};
            delta.pop();
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

    std::tuple<std::string, std::vector<node>::size_type, std::vector<node>::size_type>
    partial_merge_str(std::string_view basic_str, const std::vector<node> &delta, const std::vector<node>::size_type index)
    {
        return {{}, {}, {}};
    }
};
#ifndef XERRADA_COLUMNARDATABASE_HPP
#define XERRADA_COLUMNARDATABASE_HPP

#include <type_traits>
#include <vector>
#include <tuple>

template <typename ...Type>
class Database {
private:
    static_assert((... && std::is_fundamental_v<Type>));
    std::tuple<std::vector<Type>...> columns;
    template <std::size_t... I>
    void emplace(std::index_sequence<I...>, std::tuple<Type...> data) {
        ((0, std::get<I>(columns).emplace_back(std::get<I>(data))), ...);
    }

public:
    template <int Column>
    const auto & getColumn() {
        return std::get<Column>(columns);
    }

    void emplace(Type... data) {
        emplace(std::index_sequence_for<Type...>{}, std::make_tuple(data...));
    }
};

#endif //XERRADA_COLUMNARDATABASE_HPP

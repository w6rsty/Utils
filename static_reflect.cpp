#include <type_traits>
#include <iostream>
#include <vector>

template <typename T>
using GetClassElement_0 = std::tuple_element_t<0, T>;
template <typename T>
using GetClassElement_1 = std::tuple_element_t<1, T>;
template <typename T>
using GetClassElement_2 = std::tuple_element_t<2, T>;
template <typename T>
using GetClassElement_3 = std::tuple_element_t<3, T>;
template <typename T>
using GetClassElement_4 = std::tuple_element_t<4, T>;
template <typename T>
using GetClassElement_5 = std::tuple_element_t<5, T>;
template <typename T>
using GetClassElement_6 = std::tuple_element_t<6, T>;
template <typename T>
using GetClassElement_7 = std::tuple_element_t<7, T>;
template <typename T>
using GetClassElement_8 = std::tuple_element_t<8, T>;
template <typename T>
using GetClassElement_9 = std::tuple_element_t<9, T>;

template <typename T>
std::string TypeName();
template <>
std::string TypeName<void>() { return "void"; }
template <>
std::string TypeName<int>() { return "int"; }
template <>
std::string TypeName<double>() { return "double"; }
template <>
std::string TypeName<float>() { return "float"; }
template <>
std::string TypeName<char>() { return "char"; }
template <>
std::string TypeName<bool>() { return "bool"; }
template <>
std::string TypeName<std::string>() { return "std::string"; }

// expand tuple to string
template <typename T>
struct PackedInfo;

template <typename... Args>
struct PackedInfo<std::tuple<Args...>> {
    static std::vector<std::string> Get() {
        std::vector<std::string> result;
        (..., result.push_back(TypeName<Args>()));
        return result;
    }
};
template <typename T>
struct function_traits;

template <typename Ret, typename Class>
struct function_traits<Ret(Class::*)()> {
    using ret_type = Ret;
    using class_type = Class;
};

template <typename Ret, typename Class>
struct function_traits<Ret(Class::*)() const> {
    using ret_type = Ret;
    using class_type = Class;
};

template <typename Ret, typename Class, typename Param>
struct function_traits<Ret(Class::*)(Param)> {
    using ret_type = Ret;
    using class_type = Class;
    using param_type = Param;
};

template <typename Ret, typename Class, typename Param>
struct function_traits<Ret(Class::*)(Param) const> {
    using ret_type = Ret;
    using class_type = Class;
    using param_type = Param;
};

template <typename Ret, typename Class, typename... Param>
struct function_traits<Ret(Class::*)(Param...)> {
    using ret_type = Ret;
    using class_type = Class;
    using params_type = std::tuple<Param...>;
    static constexpr size_t params_count = sizeof...(Param);
};

template <typename Ret, typename Class, typename... Param>
struct function_traits<Ret(Class::*)(Param...) const> {
    using ret_type = Ret;
    using class_type = Class;
    using params_type = std::tuple<Param...>;
    static constexpr size_t params_count = sizeof...(Param);
};

template <typename T>
struct variable_traits;

template <typename Class, typename Type>
struct variable_traits<Type(Class::*)> {
    using class_type = Class;
    using type = Type;
};

template <typename T>
struct TypeInfo;

#define BEGIN_CLASS(T) \
template <> \
std::string TypeName<T>() { return #T; } \
template <> struct TypeInfo<T> { \
    using type = T;

#define FUNCTIONS(...) using functions = std::tuple<__VA_ARGS__>;

#define VARIABLES(...) using variables = std::tuple<__VA_ARGS__>;

#define END_CLASS() };


//==================================================================

struct Entity
{
    void SetAlive(bool alive) { m_Alive = alive; }
    bool IsAlive() { return m_Alive; }
    bool IsAliveConst() const { return m_Alive; }
    void Test(int a, double b, float c, Entity e) { std::cout << a << " " << b << std::endl; }

    bool m_Alive;
};


BEGIN_CLASS(Entity)
FUNCTIONS(function_traits<decltype(&Entity::SetAlive)>,
          function_traits<decltype(&Entity::IsAlive)>, 
          function_traits<decltype(&Entity::IsAliveConst)>,
          function_traits<decltype(&Entity::Test)>)
VARIABLES(variable_traits<decltype(&Entity::m_Alive)>)
END_CLASS()

int main() {
    using type = TypeInfo<Entity>;

    using var_0 = GetClassElement_0<type::variables>;

    using func_0 = GetClassElement_0<type::functions>;
    using func_1 = GetClassElement_1<type::functions>;
    using func_2 = GetClassElement_2<type::functions>;
    using func_3 = GetClassElement_3<type::functions>;

    for (const auto& type : PackedInfo<func_3::params_type>::Get()) {
        std::cout << type << std::endl;
    }
} 
#pragma once

template<typename T_target, typename T_generator, typename... T_generators>
class generator_pipeline {
public:
    void apply(T_target& target) {
        m_gen.apply(target);
        m_pipe.apply(target);
    }
private:
    T_generator m_gen;
    generator_pipeline<T_target, T_generators...> m_pipe;
};

template<typename T_target, typename T_generator>
class generator_pipeline<T_target, T_generator> {
public:
    void apply(T_target& target) {
        m_gen.apply(target);
    }
private:
    T_generator m_gen;
};

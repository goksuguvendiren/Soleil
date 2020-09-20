//
// Created by goksu on 6/24/20.
//

#pragma once


namespace soleil::bsdfs::fresnels
{
class dielectric
{
public:
    dielectric(float eta_i, float eta_t)
        : m_eta_i(eta_i)
        , m_eta_t(eta_t)
    {}

    [[nodiscard]] float F(float cos_theta) const;

private:
    float m_eta_i;
    float m_eta_t;
};
}
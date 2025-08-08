#include "bezier.h"
#include "main.h"

#define parameterMid 32767

struct Color setColor(uint8_t r, uint8_t g, uint8_t b)
{
        struct Color res;
        res.r = r;
        res.g = g;
        res.b = b;

        return res;
}

int linear(int p0, int p1, uint16_t t)
{
        int correction, result;
        if(p0 < p1)
                correction = parameterMid;
        else
                correction = parameterMid * -1;

        result = (p1 - p0);
        result *= t;
        result += correction;
        result /= 65536;
        result += p0;

        return result;
}

int quadratic(int p0, int p1, int p2, uint16_t t)
{
        int res1, res2, res3;
        res1 = linear(p0, p1, t);
        res2 = linear(p1, p2, t);
        res3 = linear(res1, res2, t);

        return res3;
}

int cubic(int p0, int p1, int p2, int p3, uint16_t t)
{
        int res1, res2, res3;
        res1 = quadratic(p0, p1, p2, t);
        res2 = quadratic(p1, p2, p3, t);
        res3 = linear(res1, res2, t);

        return res3;
}

struct Color cubicGradient(struct CubicGradient g, uint16_t t)
{
        struct Color resultColor;
        uint16_t biasT;

        biasT = (uint16_t)cubic(0, (int)g.u0, (int)g.u1, 65535, t);

        resultColor.r = (uint8_t)cubic((int)g.rgb0.r,(int)g.rgb1.r,(int)g.rgb2.r,(int)g.rgb3.r, biasT);
        resultColor.g = (uint8_t)cubic((int)g.rgb0.g,(int)g.rgb1.g,(int)g.rgb2.g,(int)g.rgb3.g, biasT);
        resultColor.b = (uint8_t)cubic((int)g.rgb0.b,(int)g.rgb1.b,(int)g.rgb2.b,(int)g.rgb3.b, biasT);

        return resultColor;
}

struct Color biCubicGradient(struct BiCubicGradient b, uint16_t u, uint16_t v)
{
                struct CubicGradient g0,g1,g2,g3, gResult;
                struct Color cResult;

                g0.rgb0 = b.p0;
                g0.rgb1 = b.p1;
                g0.rgb2 = b.p2;
                g0.rgb3 = b.p3;
                g0.u0 = b.u0;
                g0.u1 = b.u1;

                gResult.rgb0 = cubicGradient(g0, u);

                g1.rgb0 = b.q0;
                g1.rgb1 = b.q1;
                g1.rgb2 = b.q2;
                g1.rgb3 = b.q3;
                g1.u0 = b.u0;
                g1.u1 = b.u1;

                gResult.rgb1 = cubicGradient(g1, u);

                g2.rgb0 = b.r0;
                g2.rgb1 = b.r1;
                g2.rgb2 = b.r2;
                g2.rgb3 = b.r3;
                g2.u0 = b.u0;
                g2.u1 = b.u1;

                gResult.rgb2 = cubicGradient(g2, u);

                g3.rgb0 = b.s0;
                g3.rgb1 = b.s1;
                g3.rgb2 = b.s2;
                g3.rgb3 = b.s3;
                g3.u0 = b.u0;
                g3.u1 = b.u1;

                gResult.rgb3 = cubicGradient(g3, u);

                gResult.u0 = b.v0;
                gResult.u1 = b.v1;

                cResult = cubicGradient(gResult, v);

                return cResult;
}

int createBezier(void){
        static uint8_t current = 0;
        static uint32_t progress = 0;
        struct GradientSection *g1, *g2;
        struct BiCubicGradient b;
        uint16_t t;

        g1 = &s.gradient[current];
        if(g1->span == 0) return 0;
        g2 = &s.gradient[(current + 1) % 3];

        progress += s.bezierSpeed;
        if(progress >= g1->span){
                progress = 0;
                current++;
                if(current >= 3 || s.gradient[current].span == 0)
                        current = 0;
                g1 = &s.gradient[current];
                g2 = &s.gradient[(current + 1) % 3];
        }

        t = (uint16_t)((progress * 65535UL) / g1->span);

        b.p0.r = linear(g1->gradient.p0.r, g2->gradient.p0.r, t);
        b.p0.g = linear(g1->gradient.p0.g, g2->gradient.p0.g, t);
        b.p0.b = linear(g1->gradient.p0.b, g2->gradient.p0.b, t);
        b.p1.r = linear(g1->gradient.p1.r, g2->gradient.p1.r, t);
        b.p1.g = linear(g1->gradient.p1.g, g2->gradient.p1.g, t);
        b.p1.b = linear(g1->gradient.p1.b, g2->gradient.p1.b, t);
        b.p2.r = linear(g1->gradient.p2.r, g2->gradient.p2.r, t);
        b.p2.g = linear(g1->gradient.p2.g, g2->gradient.p2.g, t);
        b.p2.b = linear(g1->gradient.p2.b, g2->gradient.p2.b, t);
        b.p3.r = linear(g1->gradient.p3.r, g2->gradient.p3.r, t);
        b.p3.g = linear(g1->gradient.p3.g, g2->gradient.p3.g, t);
        b.p3.b = linear(g1->gradient.p3.b, g2->gradient.p3.b, t);

        b.q0.r = linear(g1->gradient.q0.r, g2->gradient.q0.r, t);
        b.q0.g = linear(g1->gradient.q0.g, g2->gradient.q0.g, t);
        b.q0.b = linear(g1->gradient.q0.b, g2->gradient.q0.b, t);
        b.q1.r = linear(g1->gradient.q1.r, g2->gradient.q1.r, t);
        b.q1.g = linear(g1->gradient.q1.g, g2->gradient.q1.g, t);
        b.q1.b = linear(g1->gradient.q1.b, g2->gradient.q1.b, t);
        b.q2.r = linear(g1->gradient.q2.r, g2->gradient.q2.r, t);
        b.q2.g = linear(g1->gradient.q2.g, g2->gradient.q2.g, t);
        b.q2.b = linear(g1->gradient.q2.b, g2->gradient.q2.b, t);
        b.q3.r = linear(g1->gradient.q3.r, g2->gradient.q3.r, t);
        b.q3.g = linear(g1->gradient.q3.g, g2->gradient.q3.g, t);
        b.q3.b = linear(g1->gradient.q3.b, g2->gradient.q3.b, t);

        b.r0.r = linear(g1->gradient.r0.r, g2->gradient.r0.r, t);
        b.r0.g = linear(g1->gradient.r0.g, g2->gradient.r0.g, t);
        b.r0.b = linear(g1->gradient.r0.b, g2->gradient.r0.b, t);
        b.r1.r = linear(g1->gradient.r1.r, g2->gradient.r1.r, t);
        b.r1.g = linear(g1->gradient.r1.g, g2->gradient.r1.g, t);
        b.r1.b = linear(g1->gradient.r1.b, g2->gradient.r1.b, t);
        b.r2.r = linear(g1->gradient.r2.r, g2->gradient.r2.r, t);
        b.r2.g = linear(g1->gradient.r2.g, g2->gradient.r2.g, t);
        b.r2.b = linear(g1->gradient.r2.b, g2->gradient.r2.b, t);
        b.r3.r = linear(g1->gradient.r3.r, g2->gradient.r3.r, t);
        b.r3.g = linear(g1->gradient.r3.g, g2->gradient.r3.g, t);
        b.r3.b = linear(g1->gradient.r3.b, g2->gradient.r3.b, t);

        b.s0.r = linear(g1->gradient.s0.r, g2->gradient.s0.r, t);
        b.s0.g = linear(g1->gradient.s0.g, g2->gradient.s0.g, t);
        b.s0.b = linear(g1->gradient.s0.b, g2->gradient.s0.b, t);
        b.s1.r = linear(g1->gradient.s1.r, g2->gradient.s1.r, t);
        b.s1.g = linear(g1->gradient.s1.g, g2->gradient.s1.g, t);
        b.s1.b = linear(g1->gradient.s1.b, g2->gradient.s1.b, t);
        b.s2.r = linear(g1->gradient.s2.r, g2->gradient.s2.r, t);
        b.s2.g = linear(g1->gradient.s2.g, g2->gradient.s2.g, t);
        b.s2.b = linear(g1->gradient.s2.b, g2->gradient.s2.b, t);
        b.s3.r = linear(g1->gradient.s3.r, g2->gradient.s3.r, t);
        b.s3.g = linear(g1->gradient.s3.g, g2->gradient.s3.g, t);
        b.s3.b = linear(g1->gradient.s3.b, g2->gradient.s3.b, t);

        b.u0 = (uint16_t)linear(g1->gradient.u0, g2->gradient.u0, t);
        b.u1 = (uint16_t)linear(g1->gradient.u1, g2->gradient.u1, t);
        b.v0 = (uint16_t)linear(g1->gradient.v0, g2->gradient.v0, t);
        b.v1 = (uint16_t)linear(g1->gradient.v1, g2->gradient.v1, t);

        s.segment[0].span = 1;
        s.segment[0].renderMode = RENDER_BICUBIC;
        s.segment[0].biCubicGradient = b;

        return 1;
}

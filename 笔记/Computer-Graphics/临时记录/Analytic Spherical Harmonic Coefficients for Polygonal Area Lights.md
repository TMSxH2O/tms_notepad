# 球谐光照
 > - https://www.jianshu.com/p/d8d79401525f
 > - https://www.cxyzjd.com/article/tiao_god/111240808

## 球谐学习记录
> https://www.cxyzjd.com/article/tiao_god/111240808

$$
L(x, \vec{\omega_o}) = L_e(x, \vec{\omega_o}) + \int_Sf_r(x, \vec{\omega_i} \to \vec{\omega_o}) L(x', \vec{\omega_i}) G(x, x') V(x, x') d\omega_i
$$

- $L(x, \vec{\omega_o})$<br>the intensity reflected from position $x$ in direction $\omega_o$
- $L_e(x, \vec{\omega_o})$<br>the light emitted from $x$ by this object itself
- $f_r(x, \vec{\omega_i} \to \vec{\omega_o})$<br>the BRDF of the surface at point $x$, transforming incoming light $\omega_i$ to reflected light $\omega_i$
- $L(x', \vec{\omega_i})$<br>light from $x'$ on another object arriving along $\omega_i$
- $G(x, x')$<br>the geometric relactionship between $x$ and $x'$
- $V(x, x')$<br>a visibility test, returns 1 if $x$ can see $x'$, 0 otherwise

### 伴随勒让德多项式
伴随勒让德多项式，Associated Legendre Polynomials，其满足三条规则

1. $(l-m)P^m_l = x(2l-1)P^m_{l-1}-(l+m-1)P^m_{l-2}$
2. $P^m_m=(-1)^m(2m-1)!!(1-x^2)^{m/2}$
3. $P^m_{m+1}=x(2m+1)P^m_m$

> 其中 $x!!$ 是双阶层函数，需要将奇数和偶数分别进行阶乘：
> $$\begin{align*}(2n-1)!!&=1\times 3 \times 5 \times \cdots \times (2n-1)\\(2n)!!&=2\times 4 \times 6 \times \cdots \times (2n)\end{align*}$$

```cpp
double P(int l, int m, double x)
{
	// evaluate an Associated Legendre Polynomial P(l,m,x) at x
	double pmm = 1.0;
	if (m>0) {
		double somx2 = sqrt((1.0 - x)*(1.0 + x));
		double fact = 1.0;
		for (int i = 1; i <= m; i++) {
			pmm *= (-fact) * somx2;
			fact += 2.0;
		}
	}
	if (l == m) return pmm;
	double pmmp1 = x * (2.0*m + 1.0) * pmm;
	if (l == m + 1) return pmmp1;
	double pll = 0.0;
	for (int ll = m + 2; ll <= l; ++ll) {
		pll = ((2.0*ll - 1.0)*x*pmmp1 - (ll + m - 1.0)*pmm) / (ll - m);
		pmm = pmmp1;
		pmmp1 = pll;
	}
	return pll;
}
```

### 球谐函数
Spherical Harmonic，SH。
SH函数通常使用 $y$ 来表示：
$$
y^m_l(\theta, \omega) = \begin{cases}\sqrt{2}K^m_lcos(m\varphi)P^m_l(cos\theta), & m>0\\ \sqrt{2}K^m_lsin(-m\varphi)P^{-m}_l(cos\theta),& m<0\\K^0_lP^0_l(cos\theta),&m=0\end{cases}
$$
其中$K^m_l=\sqrt{\frac{(2l+1)}{4\pi}\frac{(l-|m|)!}{(1+|m|)!}}$
> 因为对球谐光照而言，其中的 $m$ 将涉及到负数的情况，使用 $P^m_l$ 时，需要将 $m$ 转换为正数。

```cpp
double K(int l, int m)
{
	// renormalisation constant for SH function
	double temp = ((2.0*l + 1.0)*factorial(l - m)) / (4.0*MYPI*factorial(l + m));
	return sqrt(temp);
}
double SH(int l, int m, double theta, double phi)
{
	// return a point sample of a Spherical Harmonic basis function
	// l is the band, range [0..N]
	// m in the range [-l..l]
	// theta in the range [0..Pi]
	// phi in the range [0..2*Pi]
	const double sqrt2 = sqrt(2.0);
	if (m == 0) return K(l, 0)*P(l, m, cos(theta));
	else if (m>0) return sqrt2*K(l, m)*cos(m*phi)*P(l, m, cos(theta));
	else return sqrt2*K(l, -m)*sin(-m*phi)*P(l, -m, cos(theta));
}
```

计算球谐光照的值的方法，其中的 `SH_polar_fn` 一种自定义的表示各个方向光照强度的函数，通过传入对应的球面坐标，得到对应位置上的光强度值。

```cpp
typedef double(*SH_polar_fn)(double theta, double phi);
void SH_project_polar_function(SH_polar_fn fn, const SHSample samples[], double result[])
{
	const double weight = 4.0*MYPI;
	// for each sample
	for (int i = 0; i<n_samples; ++i) {
		double theta = samples[i].sph.x;
		double phi = samples[i].sph.y;
		for (int n = 0; n<n_coeff; ++n) {
			result[n] += fn(theta, phi) * samples[i].coeff[n];
		}
	}
	// divide the result by weight and number of samples
	double factor = weight / n_samples;
	for (int i = 0; i<n_coeff; ++i) {
		result[i] = result[i] * factor;
	}
}
```

### 漫反射非阴影传输函数
$$
L(x, \omega_o) = \int_Sf_r(x, \omega_o, \omega_i)L_i(x, \omega_i)H(x,\omega_i)d\omega_i
$$

- $L(x,\omega_o)$<br>the amount of light leaving point $x$ along vector $\omega_o$
- $f_r(x,\omega_o,\omega_i)$<br>The BRDF at point $x$
- $L_i(x,\omega_i)$<br>incoming light at point $x$ along vector $\omega_i$
- $H(x,\omega_i)$<br>the $gemetric$ or $cosine term$, as described earlier.
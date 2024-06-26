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

### 伴随勒格朗日多项式
伴随勒让德多项式，Associated Legendre Polynomials，其满足三条规则

1. $(l-m)P^m_l = x(2l-1)P^m_{l-1}-(l+m-1)P^m_{l-2}$
2. $P^m_m=(-1)^m(2m-1)!!(1-x^2)^{m/2}$
3. $P^m_{m+1}=x(2m+1)P^m_m$

> 其中 $x!!$ 是双阶层函数，需要将奇数和偶数作为两种不同情况处理：
> $$\begin{align*}(2n-1)!!&=1\times 3 \times 5 \times \cdots \times (2n-1)\\(2n)!!&=2\times 4 \times 6 \times \cdots \times (2n)\end{align*}$$

paper中有提到，使用第一条规则得到的值更加准确，因此，此处尽可能使用了第一条规则来计算最终的值。（第三条规则虽然可以提高 `l`，但是为了保证最后结果更加精准，只使用了一次）

```cpp
double P(int l, int m, double x)
{
	// evaluate an Associated Legendre Polynomial P(l,m,x) at x
	double pmm = 1.0;
	if (m>0) {
		// 首先使用的是第二条规则，来计算得到 Pmm
		double somx2 = sqrt((1.0 - x)*(1.0 + x));
		double fact = 1.0;
		for (int i = 1; i <= m; i++) {
			pmm *= (-fact) * somx2;
			fact += 2.0;
		}
	}
	if (l == m) return pmm;
	// 为了保证结果更加精准，只使用一次第三条规则
	double pmmp1 = x * (2.0*m + 1.0) * pmm;
	if (l == m + 1) return pmmp1;
	// 使用第一条规则
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
y^m_l(\theta, \omega) = \begin{cases}\sqrt{2}K^m_lcos(m\varphi)P^m_l(cos\theta), & m>0\\ \sqrt{2}K^{|m|}_lsin(|m|\varphi)P^{|m|}_l(cos\theta),& m<0\\K^0_lP^0_l(cos\theta),&m=0\end{cases}
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

对有颜色的图像处理，通常的方式是分别处理图像的各个通道，生成对应颜色的SH值，之后分别就进行处理（带入上方的 `SH_polar_fn`）。

### 漫反射非阴影传输函数
$$
L(x, \omega_o) = \int_Sf_r(x, \omega_o, \omega_i)L_i(x, \omega_i)H(x,\omega_i)d\omega_i
$$

- $L(x,\omega_o)$<br>the amount of light leaving point $x$ along vector $\omega_o$
- $f_r(x,\omega_o,\omega_i)$<br>The BRDF at point $x$
- $L_i(x,\omega_i)$<br>incoming light at point $x$ along vector $\omega_i$
- $H(x,\omega_i)$<br>the $gemetric$ or $cosine term$, as described earlier.

# Paper
对论文中的部分内容记录

## Zonal Harmonics

在论文中，使用到了 `Zonal Shperical Harmonics` 来计算，可以对 `Legendre Polynomials` 进行变形：
$$Z^m_l(\omega,\varphi)=P^m_l(cos\omega)$$
> 其中，m的相等，可以简写

之后，使用到了另一个论文中的结论，
$$Y^l_m(\omega, \varphi)=\sum_{d}\alpha^m_{l,d}Y^0_l(\omega, \varphi)$$
其中的 $\alpha^m_{l,d}$ 表示的是一组权重信息，在 `Spherical Harmonics` 的各阶与其 $m=0$ 有一定的对应关系。（似乎对应的关系在对应的文档中已给出）

因此，之前的 $Y^m_l(\omega, \varphi) = K^{|m|}_lP^{m}_l(cos\omega)\int(|m|\varphi)$ 可以简化，只求出其中 $m=0$ 的值，因此式子可以进行简化：
$$Y^0_l(cos\omega)=\sqrt{\frac{2l+1}{4\pi}}P^0_l(cos\omega)$$
原式中，$\int(|m|\varphi)$ 在 $m=0$ 时，得到的值为1。同时，带入了之前得到的 `Zonal Spherical Harmoncis` 和 `Legendre Polynomials` 的关系，最后的值只与 $\omega$ 有关，因此可以减少参数 $\varphi$。

之后，整理在此处的 `Legendre Polynomials` 的几个关系式：

$$
\begin{align*}\frac{z^2-1}{l}P_l'(z)&=zP_l(z)-P_{l-1}\\lP_l(z)&=(2l-1)zP_{l-1}(z)-(l-1)P_{l-2}(z)\\(2l+1)P_l(z)&=P'_{l+1}-P'_{l-1}(z)\end{align*}
$$
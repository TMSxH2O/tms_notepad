> 学习 timlly博客 [由浅入深PBR的原理和实现](https://www.cnblogs.com/timlly/p/10631718.html)

# 1. 概念

PBR，Physically Based Rendering，基于物理的渲染。它是利用现实世界的现象和理论，根据数学方法推导或简任或模拟出一系列渲染方程，并依赖计算机硬件和图形API渲染出拟真画面的技术.

## 1.1. 与物理渲染的区别

Physial Rendering 最终的目标。即物理现实世界中的，根据真实物理性质在视网膜上形成画面。但基于当下的硬件条件，还远远无法达到物理计算所需，正国如此，才只能叫基于物理的渲染（拟合）

# 2. PBR基础理论和推导

满足以下条件的光照模型才能称之为PBR光照模型：
1. 基于微平面模型（Be based on the 	microfacet surface model) 
2. 能量守衡（Be energy conserving). 
3. 使用基于物理的BRDF(Use a physially based BRDF). 

## 2.1. 微平面（次表面）

微平面理论认为，在微观上所有的平面都是由无数不平整的小平面组成。因此光线射入这些微小平面后，都分产生镜面反射。同时，越是粗糙的平面，其法线分布越是无序，反射出的光线也就越是杂乱；反之，光滑平面的法线分布有序，反射出的光线方向趋向于平行。

这里为了描述法线分布情况，也就是微平面方向和平面一致的概率。这里使用光线和视线的半角向量。

代码中，半角向量计算所下。
$$
h = \cfrac{l + v}{||l+v||}
$$

> 这里的半角向量和半程向量应该不是同一个东西，因为半程向量表示的是入射和出射的中间向量。当然，也有可能是同一个概念的不同应用

使用 `GLSL` 代码实现：

```glsl
vec3 light_div = normalize(light_pos - FragPos);
vec3 view_dir  = normalize(view_pos - FragPos);
vec3 halfway_dir = normalize(light_dir + view_dir);
```

## 2.2. 能量字衡 Energy Conservation 

在微平面的理论中，采用的是进似的能量守衡：出射的总能量不高于入射的总能量（自发光材质除外）。但因为此处的拟合操作只能是基本符合能量守恒规律，但为了能够达成更加准确的能量守恒，需要将光照分为两个部分：镜面反射（specular）和漫反射（diffuse）。

在现实世界中，光照射到平面时，会分成反射（reflection）和折射（refractim）两部分。反射的部分会直接从平面离开，而折射的部分会进入到物体内部，经过多次反射后，从其他位置离开。

> 如果考虑菲涅尔现象的话，这里也可能只发生反射。
> 
> 金属 Metallic 可以吸收所有折射光，故金属只有镜面反射。

使用代码表示反射光线的两个部分，可以描述为：

```glsl
// 反射光的占比
float kS = CalculateSpecularComponent(...);
// 折射光的占比
float kD = 1.0 - kS;
```

## 2.3. 反射方程ReflectanceEquation

$$
L_o(p,\omega_o)=\int_{\Omega}f_r(p,\omega_i,\omega_o)L_i(p_i,\omega_i)n\cdot \omega_id\omega_i
$$

> $L$，辐射率，Radiance，表示通过单位面积单位立体角的辐射通量$\Phi=\cfrac{dQ}{dt}$。
> $$L=\cfrac{d\Phi}{d\omega dA_\bot}$$
> $\omega$，立体角，是二维弧度的护展，1球面等于单位球体的表面积。
 $$\omega=\cfrac{S}{r^2}$$

辐射率是一个区域内光照量的辐射学度量，光照方向与平面法线的夹角越大，入射的能量越弱。因此，用 $\theta$ 表示光照与平面法线的夹角，就可以用以来表示不同方向上的射入能量。（此处是建立在各向同性的基础上）

```glsl
float cos_theta = dot(light_dir, N);
```

如果将立体角心和区域 $A$ 都看作无限小，就相当于是直接把辐射率视为一束光线打到了空间上的一个点。基于这样的假设，接可以把立体角转化为某个方向向量，将面积 $A$ 转化成点 $P$。因此片面着色器中，一般直接用辐射率来计算单光线对面片的贡献。

实际上，当谈及光的辐射率时，通常只关注进入点P的光线，这些光线的辐射度总和称为辐照度Irradiance。

通过以上的推导，方程的含义就变成了计算某个点 $p$ 在上面球面内各个方向上采样到的光照之合（这里暂时忽略了 $f_r$ 的计算）。

> 这里的 $f_r$ 就是BRDF计算，可以视为对各个角度下的光线加权。

## 2.4. 双向反射分布函数

BRDF，Bidirectional Reflectance Distribution Function，是使用入射光方向 $\omega_i$ 作为输入参数的函数，输出参数为出射光 $\omega_o$，表面法线为 $n$，变量 $\alpha$ 示的是微平面的粗糙度。

> Blim-Phong 和 BRDF 的区别：
> Blim-Phong 和 BRDF 都有使用到 $\omega_i$ 和 $\omega_o$ 作为参数，但前者并没有遵守能量守恒。

BRD下有多不同的实现，但目前几乎所有的**实时**渲染管线使用的都是 *Cook-Torrance BRDF*。Cook-Torrance BRDF 分为镜面反射和漫反射二两个部分。

其中漫反射反射出的能量被称为伦勃漫反射 *Lambertion Diffuse*，它等于一个恒等式：$f_{lambert}=\cfrac{c}{\pi}$。

> 伦勃漫反射 $f_{lambert}=\cfrac{c}{\pi}$<br>其中 $c$ 代表的是 *Albedo* 或者表面颜色，类似漫反射表面纹理。除以 $\pi$ 是为了规格化漫反射光，为后期的 *BRDF* 积分做准备。

BRDF 高光（镜面反射）计算：
$$f_{cook-torrance}=\cfrac{DFG}{4(\omega_o\cdot n)(\omega_i\cdot n)}$$

*Cook-Torrance* 镜面反射 BRDF 由三个函数构成：

1. D，Normal Distribution Function，NDF<br>法线分布函数，估算在受到表面粗糙度的影响下，取向方向和中间向量一致的微平面数量。
2. F，Fresnel equation，菲涅尔方程<br>描述的是在不同的表面角下表面反射的比率。<br>菲涅尔现象，表现为当观察方向与平面法线夹角越大时，反射的占比越高。
3. G，Geometry function，几何函数<br>描述了为平面自成阴影的属性。当一个平面相对比较粗糙的时候，平面上的微平面有可能挡住其他微平面，从而减小表面所反射的光线。<br>![PBR-GeometryFunction-GeometryObstruction](PBR-GeometryFunction-GeometryObstruction.jpg)

目前，这三种函数已经有大量不同的实现，也都有各自的侧重。这里记录UE4中选择的函数：
- D 使用的是 *Trowbridge-Reitz GGX* 
	$$NDF_{GGXTR}(n,h,\alpha)=\cfrac{\alpha^2}{\pi((n\cdot h)^2(\alpha^2-1)+1)^2}$$
	$\alpha$：粗糙度；$n$：表面法线；$h$：半程向量
- F 使用的是 *Fresnel-Schlick* 近似法（Approximation）
	$$F_{Schlick}(h,v,F_o)=F_o+(1-F_o)(1-h\cdot v)^5$$
	$v$：观察方向；（代码中，最后的 $h\cdot v$ 写的是 $n\cdot v$ 不确定这里是不是公式写错了）
- G 使用的是 *Schlick-GGX*
	$$G_{ShlickGGX}(n,v,k)=\cfrac{n\cdot v}{(n\cdot v)(1-k)+k}$$
	$k$：是通过 $\alpha$ 推导出的值：
	$$k_{direct}=\cfrac{(\alpha+1)^2}{8} \quad k_{IBL}=\cfrac{\alpha^2}{2}$$
	这里需要考虑两个方向的影响，视线方向（几何遮挡）和光线方向（几何阴影），这里会组合两者的遮蔽效果，得到最终的公式：
	$$G(n,v,l,k)=G_{SchlickGGX}(n,v,k) G_{SchlickGGX}(n,l,k)$$
	$l$：光线方向

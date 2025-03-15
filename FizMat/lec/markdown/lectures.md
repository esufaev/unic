#y2025 
---
# Лекции:
1. [[#Классификация и приведение к каноническому виду ДУ II порядка]]
2.  coming soon
> Курс читается по книгам *Тихонова* и *Самарского* "Уравнения Математической Физики"
---

# Классификация и приведение к каноническому виду ДУ II порядка
> [!note] Уравнение в частных производных II порядка
> Так называют уравнения вида
> $\sum_{i,j=1}^{n}a_{ij}\frac{ \partial ^2u }{ \partial x_{i} \partial x_{j} } + \sum_{i=1}^{n}b_{i}\frac{ \partial u }{ \partial x_{i} }+cu+f(x) = 0$
> Где
> $u=u(x_{1}, \dots,x_{n}), c=c(x), a_{ij}=a_{ij}(x), b_{i}=b_{i}(x)$
> Если же 
> $u,c,a_{ij},b_{i}=\text{const} ~ \forall j,i$
> То *уравнение имеет постоянные коэффициенты*

## Канонические ДУ II порядка (случай $n=2$)
Мы работаем с уравнениями вида
$$a_{11}(x, y)\frac{ \partial ^2u }{ \partial x^2 }+2a_{12}\frac{ \partial ^2u }{ \partial x \partial y }+a_{22}\frac{ \partial ^2 }{ \partial y^2 }+F\left( f(x,y), \frac{ \partial u }{ \partial x }, \frac{ \partial u }{ \partial y }, u \right)=0$$
> [!warning] Будем использовать следующие сокращения:
> $u_{x}'=\frac{ \partial u }{ \partial x },u_{xx}'=\frac{ \partial ^2u }{ \partial x^2 },u_{xy}'=\frac{ \partial ^2u }{ \partial x \partial y }$

Исследуем как следует такие уравнения. Сделаем замену переменных:
$$\xi=\phi(x,y), ~ \eta=\psi(x,y)$$
Причем эти функции должны быть линейно независимы:
$$\underbrace{ \mathfrak{J} }_{ \text{Якобиан} }=
\begin{vmatrix}
\frac{ \partial \phi }{ \partial x } & \frac{ \partial \phi }{ \partial y }  \\
\frac{ \partial \psi }{ \partial x } & \frac{ \partial \psi }{ \partial y } 
\end{vmatrix} \neq 0 $$

Выведем несколько полезных формул:
$$u_{xy}=u_{\xi \xi}'\xi_{x}'\xi_{y}'+u_{\xi \eta}(\xi_{x}'\eta_{y}'+\xi_{y}'\eta_{x}')+u_{\eta \eta}'\eta_{x}'\eta_{y}'+u_{\eta}'\eta_{xy}'+u_{\xi}'\xi_{xy}'$$
Или, частный случай
$$u_{xx}'=u_{\xi \xi}'\xi_{x}^2+2u_{\xi \eta}'\xi_{x}'\eta_{x}'+u_{\eta \eta}'\eta_{x}^2+u_{\xi}'\xi_{xx}'+u_{\eta}'\eta_{xx}'$$

Подставляя это в исходное уравнение получаем:
$$u_{\xi \xi}' \overbrace{[a_{11}\xi_x'^2 + a_{12} \xi_x' \xi_y' + a_{22} \xi_y'^2]}^{\tilde{a}_{11}} + u_{\eta \eta}' \overbrace{[a_{11} \eta_x'^2 + 2a_{12} \eta_x' \eta_y' + a_{22} \eta_y'^2]}^{\tilde{a}_{22}} +$$
$$+ u_{\eta \xi}' \overbrace{[2a_{11} \xi_x' \eta_x' + 2a_{22}\xi_y' \eta_y' + 2a_{12} ( \xi_x' \eta_y' + \xi_y' \eta_x' )]}^{\tilde{a}_{12}} + \tilde{F} (\xi, \eta, u, u_\xi', u_\eta') = 0$$

Итого, получаем:
$$a_{11} (\xi_x)^2 + 2 a_{12} \xi_x \xi_y + a_{22} (\xi_y)^2 = 0$$
$$a_{11} \left(\dfrac{\xi_x}{\xi_y}\right)^2 + 2 a_{12} \left(\dfrac{\xi_x}{\xi_y}\right) + a_{22} = 0$$
Положим $\xi(x, y) = \text{const}$, выразим тогда $y = y(x)$. Отсюда получаем $d\xi = \xi_x dx + \xi_y dy \Rightarrow \dfrac{dy}{dx} = -\dfrac{\xi_x}{\xi_y}$ (как по теореме о неявной функции)
Значит имеем:
$$a_{11} \left(\dfrac{dy}{dx}\right)^2 + 2a_{12} \left(\dfrac{dy}{dx}\right) + a_{22} = 0$$

Потом можно получить характеристическое уравнение:
> [!tip] Характеристическое уравнение исходного выражения
$a_{11} (dy)^2 - 2 a_{12} dy dx + a_{22} (dx)^2 = 0$ 

Далее выразим $\frac{dy}{dx}$:
$$\dfrac{dy}{dx} = \dfrac{a_{12} \pm \overbrace{\sqrt{(a_{12})^2 - a_{11} a_{22}}}^{D}}{a_{11}}$$

В зависимости от $D$ определяется тип ДУ
> [!note] Виды канонических ДУ II порядка
> - $D > 0$ — *гиперболический* тип
> - $D = 0$ — *параболический* тип
> - $D < 0$ — *эллиптический* тип

Для каждого из них определен свой порядок действий:
1. Делаем замену $\xi=\phi(x,y)=c,~\eta=\psi(x,y)=\text{const}$ 
2. У нас единственное решение $\xi=\phi(x,y)=\text{const}$. Тогда вводим вторую, линейно независимую с $\phi(x,y)$ функцию $\eta=\psi(x,y)=\text{const}$
3. У нас будет пара сопряженных комплексных чисел $\underbrace{ {\phi(x,y)} }_{ \xi(x,y) }\pm i \underbrace{ \psi(x,y) }_{ \eta(x,y) }=\text{const}$

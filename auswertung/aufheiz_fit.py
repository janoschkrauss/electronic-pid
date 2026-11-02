import numpy as np, pandas as pd, matplotlib.pyplot as plt
from scipy.optimize import curve_fit

d = pd.read_csv("serie_2_Ki10.00.csv")
h = d[d["Tset"] == 0]                    # nur Aufheizphase (Luefter aus)
t, T = h["t"].values, h["T"].values

def modell(t, T0, Tinf, tau):            # T(t) = Tinf - (Tinf - T0) * exp(-t/tau)
    return Tinf - (Tinf - T0) * np.exp(-t / tau)

p, cov = curve_fit(modell, t, T, p0=[T[0], T[-1], 200])
s = np.sqrt(np.diag(cov))
print(f"T0   = {p[0]:.2f} +- {s[0]:.2f} C")
print(f"Tinf = {p[1]:.2f} +- {s[1]:.2f} C")
print(f"tau  = {p[2]:.0f} +- {s[2]:.0f} s")
print(f"Delta T = {p[1] - p[0]:.2f} K, Streuung der Residuen {np.std(T - modell(t, *p)):.2f} K")

plt.plot(t, T, ".", ms=2, label="Messung")
plt.plot(t, modell(t, *p), label=rf"Fit: $\tau$ = {p[2]:.0f} s, $T_\infty$ = {p[1]:.2f} °C")
plt.axhline(p[1], color="gray", ls="--", lw=0.8)
plt.xlabel("t [s]"); plt.ylabel("T [°C]"); plt.legend()
plt.tight_layout(); plt.savefig("aufheiz.png", dpi=150); plt.show()

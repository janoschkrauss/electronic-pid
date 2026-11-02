import numpy as np, pandas as pd, matplotlib.pyplot as plt
from scipy.optimize import curve_fit

T25 = 298.15
R25 = 10.0                       # eigener NTC: 10 kOhm bei 25 C (in kOhm)
B_alt = 3950.0                   # bisher angenommenes B

d = pd.read_csv("kalib.csv")
R    = d["R_kOhm"].values
Tref = d["Tref"].values + 273.15

# Unsicherheit der Referenz (Grove): R25 = 100k +- 1 %, B = 4250..4299 K
B_g, sB_g, sR_g = 4275.0, 25.0, 0.01
sT_ref = np.sqrt((Tref**2 / B_g * sR_g)**2
                 + (Tref**2 * (1/Tref - 1/T25) / B_g * sB_g)**2)

def T_ntc(R, B):                 # Temperatur in K aus Widerstand
    return 1.0 / (np.log(R / R25) / B + 1.0 / T25)

popt, pcov = curve_fit(T_ntc, R, Tref, p0=[B_alt])
B, sB_stat = popt[0], np.sqrt(pcov[0, 0])
# Referenzfehler ist systematisch (alle Punkte gleich verschoben) -> Fit mit Tref + sT_ref wiederholen
B_hoch = curve_fit(T_ntc, R, Tref + sT_ref, p0=[B_alt])[0][0]
sB_sys = abs(B_hoch - B)
sB = np.hypot(sB_stat, sB_sys)
print(f"B (kalibriert) = {B:.0f} +- {sB:.0f} K   (stat {sB_stat:.0f}, sys {sB_sys:.0f})")
print(f"Referenz-Unsicherheit: {sT_ref.min():.2f} .. {sT_ref.max():.2f} K")
print(f"Abweichung vorher : {np.mean(T_ntc(R, B_alt) - Tref):+.2f} K (Mittel)")
print(f"Abweichung nachher: {np.mean(T_ntc(R, B) - Tref):+.2f} K (Mittel), "
      f"Streuung {np.std(T_ntc(R, B) - Tref):.2f} K")

fig, ax = plt.subplots(2, 1, sharex=True, figsize=(7, 6))
ax[0].plot(d["t"], d["Tref"], label="Grove (Referenz)")
ax[0].plot(d["t"], T_ntc(R, B_alt) - 273.15, label=f"NTC, B = {B_alt:.0f} K")
ax[0].plot(d["t"], T_ntc(R, B) - 273.15, label=f"NTC, B = {B:.0f} K (kalibriert)")
ax[0].set_ylabel("T [°C]"); ax[0].legend()
ax[1].plot(d["t"], T_ntc(R, B_alt) - Tref, label="vorher")
ax[1].plot(d["t"], T_ntc(R, B) - Tref, label="nachher")
ax[1].axhline(0, color="k", lw=0.5)
ax[1].set_xlabel("t [s]"); ax[1].set_ylabel("T_NTC - T_ref [K]"); ax[1].legend()
plt.tight_layout(); plt.savefig("kalib.png", dpi=150); plt.show()

import numpy as np, pandas as pd, matplotlib.pyplot as plt

T_HOCH, T_TIEF = 25.8, 25.4
d = pd.read_csv("alles.csv")

# ---- Task 23: Drehzahl gegen Duty (Phase 1, letzte 2 von 4 s je Stufe) ----
s = d[d["phase"] == 1]
scan = s.groupby("duty")["rpm"].apply(lambda r: r.iloc[-2:].mean()).reset_index()
print("Task 23: Drehzahl")
print(scan.to_string(index=False))
plt.figure(figsize=(6, 4))
plt.plot(scan["duty"], scan["rpm"], "o-")
plt.xlabel("Duty Cycle [%]"); plt.ylabel("Drehzahl [RPM]"); plt.grid(alpha=0.3)
plt.tight_layout(); plt.savefig("rpm.png", dpi=150)

# ---- Task 20: Zweipunktregler (Phasen 2-5) ----
z = d[d["phase"].between(2, 5)]
fig, ax = plt.subplots(2, 1, sharex=True, figsize=(9, 6))
for a in ax:
    a.fill_between(z["t"], 0, 1, where=z["phase"].isin([2, 4]), color="gray",
                   alpha=0.2, transform=a.get_xaxis_transform(), label="ohne Regelung")
ax[0].plot(z["t"], z["T"], label="Temperatur")
ax[0].axhline(T_HOCH, color="r", ls="--", label=f"oben {T_HOCH} °C")
ax[0].axhline(T_TIEF, color="b", ls="--", label=f"unten {T_TIEF} °C")
ax[0].set_ylabel("T [°C]"); ax[0].legend(fontsize=8)
ax[1].plot(z["t"], z["duty"], color="k")
ax[1].set_xlabel("t [s]"); ax[1].set_ylabel("Lüfter [%]")
plt.tight_layout(); plt.savefig("zweipunkt.png", dpi=150)

# ---- Task 24: Gleichgewichtstemperatur gegen Drehzahl (Phase 6, letzte 60 s) ----
g6 = d[d["phase"] == 6]
erg = []
print("\nTask 24: Gleichgewicht")
print(" duty   RPM            T_gg [C]")
for duty, g in g6.groupby("duty", sort=False):
    ende = g[g["t"] > g["t"].max() - 60]
    sT_stat = ende["T"].std()
    drift = abs(ende["T"].iloc[-5:].mean() - ende["T"].iloc[:5].mean())  # noch nicht ganz stabil?
    sT = np.sqrt(sT_stat**2 + drift**2)
    erg.append((duty, ende["rpm"].mean(), ende["rpm"].std(), ende["T"].mean(), sT))
    print(f"{duty:4.0f} %  {erg[-1][1]:5.0f} +- {erg[-1][2]:3.0f}   {erg[-1][3]:.2f} +- {sT:.2f}")
e = np.array(erg)
fig, ax = plt.subplots(1, 2, figsize=(11, 4))
ax[0].plot(g6["t"], g6["T"]); ax[0].set_xlabel("t [s]"); ax[0].set_ylabel("T [°C]")
ax0b = ax[0].twinx(); ax0b.plot(g6["t"], g6["duty"], color="gray", alpha=0.5)
ax0b.set_ylabel("Lüfter [%]")
ax[1].errorbar(e[:, 1], e[:, 3], xerr=e[:, 2], yerr=e[:, 4], fmt="o", capsize=3)
ax[1].set_xlabel("Drehzahl [RPM]"); ax[1].set_ylabel("Gleichgewichtstemperatur [°C]")
ax[1].grid(alpha=0.3)
plt.tight_layout(); plt.savefig("gleichgewicht.png", dpi=150)
plt.show()

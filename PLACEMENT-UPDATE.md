# Plaatsingscorrectie — 24 september 2026

## Uitgevoerd

- C5: pin 1 op (12,00; 101,75) mm; behuizingsmiddelpunt (14,50; 101,75) mm. De radiale condensator staat vrij van Q2, F1 en Q5. Courtyard met straal 6,50 mm toegevoegd.
- Weerstandencluster naast XIAO: kolommen met 4,00 mm steek en rijen met 2,50 mm steek. Componentoriëntatie horizontaal behouden. 17 verspringende weerstanden uitgelijnd; waarden, netnamen en componentnummers behouden.
- Massavlakken verbonden met extra vias; R54-massapad rechtstreeks aan het vlak gekoppeld en de thermische aansluitingen van J24-pin 2 op 45° gezet.
- Aansluitingen opnieuw gerouteerd en spoorrestanten verwijderd. C5 heeft een nieuwe 0,80 mm voedingsaftakking; de lokale ontkoppelcondensatoren bij de eFuse blijven staan.
- Gerbers, boringen, SMT-posities, laagplots, STEP en KiCad-renders vervangen door actuele exports.

## Controle

Schema/PCB-netvergelijking en componentvergelijking zonder afwijkingen. KiCad DRC: nul overtredingen, nul open verbindingen en nul footprintafwijkingen. STEP-intersectiecontrole: geen ongewenste componentoverlap boven 0,05 mm³. De modellen bevatten de vereenvoudigingen en montageaannamen beschreven in `3D-INSPECTIE.md`; dit is geen tolerantiesimulatie of fysieke kwalificatie.

De eerdere fabricageblokkade wegens C5-overlap is hiermee opgeheven. Gebruik uitsluitend het nieuwe pakket. Begin met één geassembleerd prototype en voer de beschreven inbedrijfstelling uit.

Exacte voor/na-posities: `3dmodels/placement-changes.json`.

# Purchasing BOM

The tracker contains **68 purchase lines**: 58 PCB part types covering all 202 PCB positions, six remote-sensor items and four system items (XIAO module, supply, four TECs and bare PCB).

## Browser tracker: clickable checkboxes

Download [order-tracker.html](order-tracker.html) and open it in a browser. It is a standalone file; no server or installation is needed. GitHub displays the source, so use **Download raw file**, then open the downloaded file.

1. Set **Build quantity**. Enter existing **Stock** before buying; the XIAO is reported as already owned, but stock starts at zero until you confirm its availability.
2. Enter **Order qty**, **Supplier**, optional supplier SKU and **Unit price EUR**. Tick **Ordered** after placing the order.
3. Fill in **Order reference**, **Expected date** and **Product URL** as useful.
4. Enter **Received qty** as items arrive. Tick **Delivered** only when the whole order has arrived. A short order or incomplete receipt remains visible in Status.
5. Use **Export progress JSON** regularly. To keep the state in this repository, save the download as `procurement/order-progress.json` and commit it. Use **Import progress JSON** to restore it on another browser or computer. Import replaces the current browser state after confirmation.

Edits are saved in browser storage, **not automatically committed to GitHub**. Browser storage for local files varies by browser and may be lost when data is cleared or the file is moved. The JSON export is the portable backup. No data is sent to a server. Search and category/status filters affect only the view; exports include all rows. No purchase is placed by the tracker.

## Excel alternative

[ITO-Controller-Order-Tracker.xlsx](ITO-Controller-Order-Tracker.xlsx) has the same initial BOM with filters, frozen headers, editable amber fields and automatic totals. **Ordered** and **Delivered** are Yes/No dropdowns in this version, not native Excel checkbox controls. Save and commit the edited workbook to retain its progress. Excel and browser progress are independent; JSON is for the browser tracker only. Browser CSV exports provide a flat snapshot, not the Excel formulas or controls.

## Calculation conventions

- Required = quantity per build × build quantity.
- Still needed = max(0, required − initial stock − received quantity). Stock excludes receipts entered in this tracker.
- Line total = order quantity × unit price, rounded to cents. A missing price stays blank; an explicitly entered zero is accepted.
- Priced order total includes all entered order quantities, including planned orders. It excludes unpriced lines, VAT and shipping; it is not a complete project budget.
- One row uses one supplier and unit price. For multiple orders of a part, combine quantities and use a weighted unit price; record the order references in Notes. Use a separate order ledger if per-shipment accounting is needed.
- Ready means enough parts are physically available for the selected build quantity. Delivered confirms the entire purchase quantity arrived; these are different states when spares were ordered.
- Extra/spare quantities can be entered in Order qty. Prices, suppliers, receipt quantities and order flags start empty or zero; no availability or prices have been assumed.

## Source and scope

Snapshot: commit [`02f71dd`](https://github.com/luciddreams/ito-controller/tree/02f71dd4e608c7d41adff49b4cf41a6e93647b64).

PCB rows group [BOM-prototype.csv](../BOM-prototype.csv) by manufacturer and exact MPN. All grouped references and specifications are retained, with a check that grouped footprints match. [external-sensor-BOM.csv](../external-sensor-BOM.csv) supplies the six remote-sensor rows. The four system rows come from the project decisions and [README](../README.md). No CAD or fabrication files are changed. The engineering CSV remains authoritative for component selection; this tracker does not automatically refresh after design changes.

Still outside the priced/orderable list: exact NTC probes, fan and cooling assembly, thermal interface materials, glass/coating and busbar bonding materials, safety thermostats, switches, remaining cable harnesses/mating connectors, mains cable, enclosure and mounting hardware. Specify these against the assembly instructions before treating the list as a complete installation BOM. Do not buy substitutions based only on nominal value: footprint, voltage, tolerance and power ratings matter.

Validation: PCB quantities reconcile to 202 positions; spreadsheet quantity/cost/partial-receipt formulas and browser calculation/JSON validation paths were exercised. Spreadsheet previews were inspected. Native Excel interaction and the browser UI were not run in this environment.

For your requirements, I'd recommend building **StockPilot – Desktop Inventory Management System**.

It is a good balance between simplicity and presentation quality. It demonstrates C++ OOP, database integration, GUI development and real-world functionality without becoming too large for an individual college project.

We'll build it as an actual Windows desktop application that works offline, stores data locally and can be demonstrated without an internet connection.

# **1. Project overview**

[Inventory Management Dashboard by Delisas: UX/UI & SaaS Agency on Dribbble](https://images.openai.com/static-rsc-4/UM9S2xB2FisxRQdYJZV4hS_dEKN_owOLOnXGGW6yfdqlFfMNToHVTyUvZVUKuOEJsJoyVZ-WUvjGF0fDk1nkXcrAYgOX6oBigjrMi0q0_BxeRA63cEXS4kIBqskiF0wZUJt1jcJ-AFdrteJDgnSQqgnx1kQHH334MK9BfGCLIIw?purpose=inline)

[dribbble.com](https://dribbble.com/shots/27053072-Inventory-Management-Dashboard?utm_source=chatgpt.com)

# **StockPilot**

Desktop Inventory Management System

**Project objective**

To develop a standalone C++ desktop application that enables small businesses to manage products, monitor inventory, record stock movements and maintain supplier information using a locally stored database.

Offline capable

Database integrated

GUI application

Individual project

## **2. Technology stack**

| Component            | Technology    |
| -------------------- | ------------- |
| Programming language | C++17         |
| GUI framework        | Qt 6 Widgets  |
| Database             | SQLite        |
| Database integration | Qt SQL        |
| IDE                  | Qt Creator    |
| Build system         | CMake         |
| Platform             | Windows 10/11 |

**Why this combination?**

- Qt Widgets provides native desktop GUI components.
- SQLite stores everything in a single `.db` file.
- Qt SQL connects C++ directly to SQLite.
- CMake manages the project and its dependencies.

No server, hosting or internet connection is required.

# **3. Application modules**

We'll keep the first version deliberately manageable.

## **01**

**Dashboard**

Total products, inventory value, low-stock alerts and recent activity.

## **02**

**Product Management**

Add, edit, delete, search and view products.

## **03**

**Stock Management**

Record stock additions and deductions.

## **04**

**Supplier Management**

Maintain supplier names, contact details and addresses.

## **05**

**Reports**

View inventory, stock levels and transaction history.

# **4. Database design**

We'll use SQLite with four main tables.

**Database:&#x20;**`stockpilot.db`

**products**

Main table

Stores product information and current stock.

- product_id (PRIMARY KEY)
- name
- category
- price
- quantity
- minimum_stock
- supplier_id (FOREIGN KEY)

**suppliers**

- supplier_id (PRIMARY KEY)
- name
- phone
- email
- address

**stock_transactions**

- transaction_id (PRIMARY KEY)
- product_id (FOREIGN KEY)
- transaction_type (IN / OUT)
- quantity
- transaction_date
- remarks

**categories**

- category_id (PRIMARY KEY)
- category_name

### **Database relationships**

```
       SUPPLIERS
           |
           | 1 : Many
           |
        PRODUCTS
         /    \
        /      \
       |        |
       |        |
 CATEGORIES  STOCK_TRANSACTIONS
   1 : Many      1 : Many
```

One supplier can supply multiple products. One product can have multiple stock transactions.

**Important implementation rule:** Stock changes and transaction history must be saved together in a database transaction. This prevents the inventory quantity from changing without a corresponding history record.

# **5. GUI design**

The application will have a sidebar navigation and a central content area.

[Inventory Management Dashboard by obinna ogbonna on Dribbble](https://images.openai.com/static-rsc-4/MI4oFbSLPWiVmSKezUSK2cIxc8SI7QeiYdqfHG1Nuia847Wci-DEJBBdr6m5anjuD-brrsNx8L4AIM3Z4i87qOMu7iM7td3ubIaBwca0AafdMkzVGt9My4pukU8V3O_JgxbdsYFVf9-jAsBIGtJrgoiX2inpcjtHUZMG_PsT7JQ?purpose=inline)

[dribbble.com](https://dribbble.com/shots/26319429-Inventory-Management-Dashboard?utm_source=chatgpt.com)

### **Main screens**

[Inventory Management Dashboard by Sheraz Alam on Dribbble](https://images.openai.com/static-rsc-4/P5aYQBiy_0jMs791WCa_EOWvOuCWn4V4_56FLcrVUZLOH-SL1PU0-k13Ys1p96D7lcJaO4brzLvco1Nd5V2Kic22hvHiX0ri_vX0WKCUUi4pS2g9m3QgfuHEeBE9ImYF3nLXhUqejMzq_IVdc9GLivv-ya42dHRgE3OAqQmKC88?purpose=inline)

**Dashboard**

- Total products
- Total stock units
- Total inventory valuation
- Low-stock products

[Inventory Management Platform by Victoria Grinevich on Dribbble](https://images.openai.com/static-rsc-4/vxnUFwFcNHzdlOGjMIHKszmgYZYiUrDwTSuvcfOjrFjQW7ldrgIQqh05Ns-L0I_gObEYOEvIHgxzIWSLDk5CEIbH3ebmmgqNiNbAecG4JrQXgXEx1s6-kTQN5EaPoC3nxy9-rt8Ki-SXH_w28IzFUaYXxme2uRrwvjNJkavzbLY?purpose=inline)

**Products**

- Product table
- Add/edit product dialog
- Search and category filters
- Delete confirmation

[Stock Tracking Software India — Track Stock In, Stock Out & Item Ledger | Stock Register](https://images.openai.com/static-rsc-4/BQ2ej5Q0dLK_ZkymWFmpy9Ea3M1c32CFxY1RRrSAR7Gmgmd1eL4CjQMJV_Dd67LKG4wE92sHv4zkrDLRs1CzXpeUjzBU9O_zvvom6mOGrdKDm0a6tLx27EJTyCABFNjasjAUFxv5lqZ7YDR7X4r6XrrjrWOKJ9SEsV_sSQyjaWA?purpose=inline)

**Stock Operations**

- Stock In
- Stock Out
- Quantity validation
- Transaction history

[Supplier Management System | Medius](https://images.openai.com/static-rsc-4/_stOnnT_iv0IUprAvdSs8GN4MVoXQOpFMgRb4XoQyOfErufNvuDn8OlFsCrmNQ1SxQJP63JrG0VuTbqZoPBBl1XCzLvQHa-glS2TzXu4OVQO8CoAg7Kf_VwOeKHOOPYSRyR0-PofwcljsdJ-96ncFturgr_CrfXDp3cPMgOmUqw?purpose=inline)

**Suppliers**

- Add suppliers
- Edit contact details
- View supplied products

[Rain Retail Help Center](https://images.openai.com/static-rsc-4/U8vG-Mo5INQyeiQW_-r5MijXkOPos5DtECkrlxa5XLnmBMDArr1LwwuVpLgHieWaQ4PCb-8erI5Tl57GwhvgyahKoG2wUV_wZgXS0LKpRhLs829pEpQa1DceclbJ6o3jNogwCOIn2KHeVn3DoR3M6A9ryqu5JOHn5HkDBqGEIro?purpose=inline)

**Reports**

- Inventory report
- Low-stock report
- Stock movement history

# **6. Application workflow**

The application's core functionality will follow this flow:

\#chatgpt-mermaid-\_r_f4\_{font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";font-size:16px;fill:rgb(237, 237, 237);}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#chatgpt-mermaid-\_r_f4\_ .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#chatgpt-mermaid-\_r_f4\_ .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#chatgpt-mermaid-\_r_f4\_ .error-icon{fill:rgb(48, 48, 48);}#chatgpt-mermaid-\_r_f4\_ .error-text{fill:rgb(237, 237, 237);stroke:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ .edge-thickness-normal{stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ .edge-thickness-thick{stroke-width:3.5px;}#chatgpt-mermaid-\_r_f4\_ .edge-pattern-solid{stroke-dasharray:0;}#chatgpt-mermaid-\_r_f4\_ .edge-thickness-invisible{stroke-width:0;fill:none;}#chatgpt-mermaid-\_r_f4\_ .edge-pattern-dashed{stroke-dasharray:3;}#chatgpt-mermaid-\_r_f4\_ .edge-pattern-dotted{stroke-dasharray:2;}#chatgpt-mermaid-\_r_f4\_ .marker{fill:rgb(175, 175, 175);stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_f4\_ .marker.cross{stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_f4\_ svg{font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";font-size:16px;}#chatgpt-mermaid-\_r_f4\_ p{margin:0;}#chatgpt-mermaid-\_r_f4\_ .label{font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ .cluster-label text{fill:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ .cluster-label span{color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ .cluster-label span p{background-color:transparent;}#chatgpt-mermaid-\_r_f4\_ .label text,#chatgpt-mermaid-\_r_f4\_ span{fill:rgb(237, 237, 237);color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ .node rect,#chatgpt-mermaid-\_r_f4\_ .node circle,#chatgpt-mermaid-\_r_f4\_ .node ellipse,#chatgpt-mermaid-\_r_f4\_ .node polygon,#chatgpt-mermaid-\_r_f4\_ .node path{fill:rgb(17, 40, 19);stroke:rgb(58, 132, 63);stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ .rough-node .label text,#chatgpt-mermaid-\_r_f4\_ .node .label text,#chatgpt-mermaid-\_r_f4\_ .image-shape .label,#chatgpt-mermaid-\_r_f4\_ .icon-shape .label{text-anchor:middle;}#chatgpt-mermaid-\_r_f4\_ .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ .rough-node .label,#chatgpt-mermaid-\_r_f4\_ .node .label,#chatgpt-mermaid-\_r_f4\_ .image-shape .label,#chatgpt-mermaid-\_r_f4\_ .icon-shape .label{text-align:center;}#chatgpt-mermaid-\_r_f4\_ .node.clickable{cursor:pointer;}#chatgpt-mermaid-\_r_f4\_ .root .anchor path{fill:rgb(175, 175, 175)!important;stroke-width:0;stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_f4\_ .arrowheadPath{fill:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_f4\_ .edgePath .path{stroke:rgb(175, 175, 175);stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ .flowchart-link{stroke:rgb(175, 175, 175);fill:none;}#chatgpt-mermaid-\_r_f4\_ .edgeLabel{background-color:rgb(0, 0, 0);text-align:center;}#chatgpt-mermaid-\_r_f4\_ .edgeLabel p{background-color:rgb(0, 0, 0);}#chatgpt-mermaid-\_r_f4\_ .edgeLabel rect{opacity:0.5;background-color:rgb(0, 0, 0);fill:rgb(0, 0, 0);}#chatgpt-mermaid-\_r_f4\_ .labelBkg{background-color:rgba(0, 0, 0, 0.5);}#chatgpt-mermaid-\_r_f4\_ .cluster rect{fill:rgb(48, 48, 48);stroke:rgba(255, 255, 255, 0.15);stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ .cluster text{fill:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ .cluster span{color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";font-size:12px;background:rgb(48, 48, 48);border:1px solid rgba(255, 255, 255, 0.15);border-radius:2px;pointer-events:none;z-index:100;}#chatgpt-mermaid-\_r_f4\_ .flowchartTitleText{text-anchor:middle;font-size:18px;fill:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_f4\_ rect.text{fill:none;stroke-width:0;}#chatgpt-mermaid-\_r_f4\_ .icon-shape,#chatgpt-mermaid-\_r_f4\_ .image-shape{background-color:rgb(0, 0, 0);text-align:center;}#chatgpt-mermaid-\_r_f4\_ .icon-shape p,#chatgpt-mermaid-\_r_f4\_ .image-shape p{background-color:rgb(0, 0, 0);padding:2px;}#chatgpt-mermaid-\_r_f4\_ .icon-shape .label rect,#chatgpt-mermaid-\_r_f4\_ .image-shape .label rect{opacity:0.5;background-color:rgb(0, 0, 0);fill:rgb(0, 0, 0);}#chatgpt-mermaid-\_r_f4\_ .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#chatgpt-mermaid-\_r_f4\_ .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#chatgpt-mermaid-\_r_f4\_ .node .neo-node{stroke:rgb(58, 132, 63);}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].node rect,#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].cluster rect,#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].node polygon{stroke:url(#chatgpt-mermaid-\_r_f4\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].swimlane.cluster rect{filter:none;}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].node path{stroke:url(#chatgpt-mermaid-\_r_f4\_-gradient);stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].node .outer-path{filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].node .neo-line path{stroke:rgb(58, 132, 63);filter:none;}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].node circle{stroke:url(#chatgpt-mermaid-\_r_f4\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].node circle .state-start{fill:#000000;}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].icon-shape .icon{fill:url(#chatgpt-mermaid-\_r_f4\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_f4\_ [data-look="neo"].icon-shape .icon-neo path{stroke:url(#chatgpt-mermaid-\_r_f4\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_f4\_ .node text{font-size:14px;font-weight:600;letter-spacing:normal;fill:rgb(140, 223, 173);}#chatgpt-mermaid-\_r_f4\_ .edgeLabels text{font-size:13px;font-weight:600;letter-spacing:-0.08px;fill:rgb(140, 223, 173);}#chatgpt-mermaid-\_r_f4\_ .node tspan[font-weight="normal"],#chatgpt-mermaid-\_r_f4\_ .edgeLabels tspan[font-weight="normal"]{font-weight:600;}#chatgpt-mermaid-\_r_f4\_ .edgeLabel .label rect{opacity:1;rx:13px;ry:13px;fill:rgb(2, 28, 12);stroke:rgb(26, 75, 45);stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ .node rect,#chatgpt-mermaid-\_r_f4\_ .node circle,#chatgpt-mermaid-\_r_f4\_ .node ellipse,#chatgpt-mermaid-\_r_f4\_ .node polygon,#chatgpt-mermaid-\_r_f4\_ .node path{fill:rgb(0, 55, 22);stroke:rgba(255, 255, 255, 0.1);stroke-width:1px;}#chatgpt-mermaid-\_r_f4\_ .node rect{rx:16px;ry:16px;}#chatgpt-mermaid-\_r_f4\_ .mermaid-decision .label-container{fill:rgb(2, 28, 12);stroke:rgb(26, 75, 45);stroke-dasharray:2,2;}#chatgpt-mermaid-\_r_f4\_ .edgePaths .flowchart-link{stroke:rgb(175, 175, 175);stroke-width:1px;stroke-linecap:round;stroke-linejoin:round;}#chatgpt-mermaid-\_r_f4\_ .marker{fill:rgb(175, 175, 175);stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_f4\_ :root{--mermaid-font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";}Launch StockPilotInitialize SQLite DatabaseLoad DashboardSelect ModuleManage ProductsManage SuppliersManage StockView ReportsSQLite DatabaseUpdate Stock QuantityRecord Transaction

Example scenario:

1. Add a product named Wireless Mouse.
2. Set its opening quantity to 50.
3. Record a stock-out transaction of 5 units.
4. Application updates quantity to 45.
5. Dashboard reflects the updated inventory.
6. Transaction history records the deduction.

This one workflow demonstrates GUI interaction, C++ logic and database integration.

# **7. Recommended project structure**

We'll keep the code modular rather than putting everything inside `main.cpp`.

```
StockPilot/
│
├── CMakeLists.txt
├── README.md
│
├── resources/
│   ├── icons/
│   └── styles/
│       └── style.qss
│
├── database/
│   ├── database.h
│   └── database.cpp
│
├── models/
│   ├── product.h
│   ├── supplier.h
│   └── transaction.h
│
├── services/
│   ├── productservice.h
│   ├── productservice.cpp
│   ├── stockservice.h
│   └── stockservice.cpp
│
├── ui/
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   ├── dashboard.h
│   ├── dashboard.cpp
│   ├── products.h
│   ├── products.cpp
│   ├── suppliers.h
│   ├── suppliers.cpp
│   ├── stock.h
│   ├── stock.cpp
│   ├── reports.h
│   └── reports.cpp
│
└── main.cpp
```

# **8. Implementation roadmap**

| Phase | Work                           | Priority  |
| ----- | ------------------------------ | --------- |
| 1     | Set up C++ and Qt project      | Essential |
| 2     | Configure SQLite database      | Essential |
| 3     | Build main window and sidebar  | Essential |
| 4     | Implement product CRUD         | Essential |
| 5     | Implement supplier management  | Essential |
| 6     | Implement stock operations     | Essential |
| 7     | Dashboard and reports          | Essential |
| 8     | Styling, testing and packaging | Final     |

# **9. Features to keep out of version 1**

Since this is a basic individual assignment, let's avoid unnecessary complexity.

| Feature                    | Decision           |
| -------------------------- | ------------------ |
| User authentication        | Not required       |
| Cloud synchronization      | Not required       |
| Online APIs                | Not required       |
| Barcode scanning           | Future enhancement |
| PDF export                 | Optional           |
| Graphical analytics        | Optional           |
| Automatic low-stock alerts | Include            |
| Search and filtering       | Include            |

One small schema refinement: `products` should use `category_id` as a foreign key to the `categories` table instead of storing a free-text category. This keeps the database properly relational.

# **10. What you'll submit**

Your final deliverables should be:

1. **StockPilot.exe** – standalone desktop executable.
2. **stockpilot.db** – SQLite database.
3. **Source code** – complete C++ project.
4. **Project report** – introduction, objectives, architecture, database schema, screenshots and conclusion.
5. **Presentation** – demonstration of the working application.

The executable should be packaged with the necessary Qt runtime libraries and SQLite driver so it can run on another compatible Windows machine without requiring Qt Creator.

# **11. How we'll build it**

My recommendation is to develop it in this order:

1. **Environment setup**

   Install Qt 6, Qt Creator and configure the C++ compiler and CMake.
2. **Database foundation**

   Create SQLite tables and implement database connection and initialization.
3. **Core functionality**

   Implement products, suppliers, stock operations and transaction history.
4. **Desktop interface**

   Connect all modules to the Qt GUI and build the dashboard.
5. **Testing and packaging**

   Test database persistence, calculations, validations and standalone execution.

## **One final decision before implementation**

How do you want to approach development?

**Development approach**

Build together, step by step – understand every component and write the code progressively.

Generate the complete application first, then understand and modify it.

Hybrid – build individual modules with AI assistance and review each one.

**Your preferred development environment**

VS Code

Qt Creator

Visual Studio

No preference – recommend one

Let's build StockPilot

My preference: **Hybrid development with VS Code or Qt Creator.** You'll get a working application while actually learning C++ concepts, SQL, OOP and desktop application development.
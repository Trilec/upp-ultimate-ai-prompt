# U++ Hybrid Ops‑Table + Facade Guide (Developer Handbook)

> **Purpose** — A clear, practical pattern for building U++ apps where your **model is plain data**, **behavior is chosen by a small ops table** (manual vtable), and **usage feels class‑like** via lightweight **facade** wrappers. Keeping things fast, easy to serialize/test, and aligned with U++’s value semantics (no stray `new`).

---

## 1) Why this pattern

#### Use it when:

* You have **many similar items** (shapes, media, nodes, tools) with small differences.
* The **set of operations** is mostly stable (paint, hit, serialize, open…), and you add **new types** over time.
* You want **value semantics** (`Vector<T>`, POD models), fast iteration, simple (de)serialization, and **U++ no‑`new` discipline**.


#### When class virtuals class (or CRTP) are the better choice

* You constantly add **new operations** and need compiler‑enforced overrides. (Classic virtuals/CRTP may be simpler.)
* You keep adding new operations (e.g., ExportQTF, Duplicate, Transform, BakeToMesh) and want the compiler to force every type to implement them.
* Interface churn is high: operations evolve weekly; you want one place (the base interface) to declare the contract and let compile errors find gaps.
* Behavior depends more on operation than on data layout (the same object shape, many ever-growing behaviors).
* You need polymorphic extension at plugin boundaries (new types added by external modules should fail fast if they miss ops).
* Per-type stateful behavior is common (caches, internal invariants) and fits naturally as members of derived classes.
* Teams prefer discoverability via class hierarchies (jump to Rect::ExportQTF() rather than hunt in a registry).
* You want override visibility/controls (e.g., final, override, protected helpers) and access to base utilities.
Dynamic dispatch cost is negligible versus clarity (UI tools, editors, command systems).
Heterogeneous containers are needed and natural (store Vector<One<Shape>> in U++; no custom tag/ops lookup).

Tooling & tests benefit from mocking/stubs via inheritance (swap in FakeShape that overrides one method).

Safety over speed: breaking changes should break builds (virtuals/CRTP enforce completeness automatically).

You don’t want to maintain an ops table (no index sync, no null-fn guards, no enum reorder risks).

U++-specific notes

Prefer Vector<One<Base>> (or Array<One<Base>>) for ownership; avoid raw new.

If you want compile-time enforcement but zero virtuals, use CRTP and store as Value/variant or type-erased wrappers.

Keep derived classes Moveable where possible; avoid hidden heavy copies.

**Benefits**

* **Predictable performance** (no per‑object heap; contiguous storage).
* **Serializable models** (plain data; table is global and constinit).
* **Discoverable code** (ops wired in one place; per‑type function names are a human index).
* **Ergonomics** (facade provides neat class‑like calls without losing the ops-table’s strengths).

---

## 2) Architecture at a glance

**Model (POD)** → **Ops Table (manual vtable)** → **Facade (class‑like sugar)**

```
struct Shape { ShapeType type; /* geometry & style */ }   // plain data
constinit ShapeOps OPS[COUNT];                            // function pointers per type
class ShapeView { Shape& s; /* forwards to Ops */ };      // class‑like usage
```

* **Model**: just the data you need at runtime and for saving/loading.
* **Ops Table**: small struct of function pointers; one row per type.
* **Facade**: tiny wrapper that holds a reference to the model and forwards to the ops row so call sites read cleanly.

---

## 3) Naming & file layout (stay findable)

**Files**

* `shape.h` — `struct Shape`, `enum class ShapeType`.
* `shape_ops.h/.cpp` — `struct ShapeOps`, declarations of per‑type functions, `BuildShapeOps()`, `Ops(ShapeType)` accessor.
* `shape_facade.h` — `class ShapeView` (and optional typed views like `RectView`).
* `op_paint.cpp`, `op_hit.cpp` **or** per‑type files `rect_ops.cpp`, `circle_ops.cpp` (your choice, just be consistent).

**Function naming**

* Use `Type_Action`, e.g. `Rect_Paint`, `Circle_Hit`, `Photo_Hydrate`.
* Keep names **literal to the action**. Prefer clarity over cleverness.

**Enum safety**

* Index arrays with `static_cast<int>(t)` (or `std::to_underlying(t)` on C++23). Add a `static_assert` on table size.

---

## 4) Minimal skeleton (copy‑paste)

> The following is intentionally small and heavily commented. Paste, build, and extend.

```cpp
// shape.h — model (plain data)
#pragma once
#include <CtrlLib/CtrlLib.h>
using namespace Upp;

enum class ShapeType : int { Rect, Circle, Line, COUNT };

struct Shape {
    ShapeType type{};            // tag: which ops row applies
    Point     p1, p2;            // geometry (meaning depends on type)
    Color     fill{};            // interior color
    Color     stroke{};          // outline color
    int       pen = 2;           // outline width in px
    // Keep this POD: easy to copy, sort, serialize
};
```

```cpp
// shape_ops.h — ops row + accessor
#pragma once
#include "shape.h"

using PaintFn = void(*)(const Shape&, Draw&);
using HitFn   = bool(*)(const Shape&, Point);
using NameFn  = const char*(*)(const Shape&);

struct ShapeOps {
    PaintFn paint{};   // draw this shape on a Draw surface
    HitFn   hit{};     // hit test at a point (ui)
    NameFn  name{};    // optional: label for UI
};

// Per‑type function declarations (grep‑friendly)
void Rect_Paint(const Shape&, Draw&);   bool Rect_Hit(const Shape&, Point);
void Circle_Paint(const Shape&, Draw&); bool Circle_Hit(const Shape&, Point);
void Line_Paint(const Shape&, Draw&);   bool Line_Hit(const Shape&, Point);

const ShapeOps& Ops(ShapeType t);       // accessor (defined in .cpp)
```

```cpp
// shape_ops.cpp — build the table once; expose Ops()
#include "shape_ops.h"

static constinit std::array<ShapeOps, (int)ShapeType::COUNT> SHAPE_OPS = []{
    std::array<ShapeOps, (int)ShapeType::COUNT> a{}; // zero‑init: safer in debug
    a[(int)ShapeType::Rect]   = { &Rect_Paint,   &Rect_Hit,   +[](auto&){return "Rect";} };
    a[(int)ShapeType::Circle] = { &Circle_Paint, &Circle_Hit, +[](auto&){return "Circle";} };
    a[(int)ShapeType::Line]   = { &Line_Paint,   &Line_Hit,   +[](auto&){return "Line";} };
    return a;
}();

const ShapeOps& Ops(ShapeType t) {
    return SHAPE_OPS[(int)t];
}

#ifdef _DEBUG
// Guard: ensure every row is filled (helps when adding new types)
struct _OpsGuard {
    _OpsGuard(){
        for(int i=0;i<(int)ShapeType::COUNT;++i){
            const auto& r = SHAPE_OPS[i];
            ASSERT(r.paint && r.hit); // add checks if you add more fields
        }
    }
} _ops_guard;
#endif
```

```cpp
// shape_facade.h — class‑like sugar (no heap, no virtuals)
#pragma once
#include "shape_ops.h"

class ShapeView {
    Shape& s; // hold a reference: we do NOT own, we just forward
public:
    explicit ShapeView(Shape& s) : s(s) {}

    // Class‑like methods (readable at call sites)
    void        Paint(Draw& w) const { Ops(s.type).paint(s, w); }
    bool        Hit(Point pt)  const { return Ops(s.type).hit(s, pt); }
    const char* Name()         const { return Ops(s.type).name ? Ops(s.type).name(s) : "Shape"; }

    // Direct model access (controlled mutability)
    ShapeType   Type()  const { return s.type; }
    Point&      P1()          { return s.p1; }  // callers may mutate through the view
    Point&      P2()          { return s.p2; }
    Color&      Fill()        { return s.fill; }
    Color&      Stroke()      { return s.stroke; }
    int&        Pen()         { return s.pen; }
};
```

```cpp
// op_paint.cpp — short, focused per‑type implementations
#include "shape_ops.h"

static inline Rect MakeRectTLBR(Point a, Point b) {
    int x1=min(a.x,b.x), y1=min(a.y,b.y), x2=max(a.x,b.x), y2=max(a.y,b.y);
    return RectC(x1,y1,x2-x1,y2-y1);
}
static inline bool HitSeg(Point a, Point b, Point p, int tol=4) {
    double vx=b.x-a.x, vy=b.y-a.y; double L2=vx*vx+vy*vy; if(L2<1) return false;
    double t=((p.x-a.x)*vx+(p.y-a.y)*vy)/L2; t=Clamp(t,0.0,1.0);
    double nx=a.x+t*vx, ny=a.y+t*vy; double dx=p.x-nx, dy=p.y-ny;
    return dx*dx+dy*dy <= tol*tol;
}

void Rect_Paint(const Shape& s, Draw& w) {
    // draw a filled rect + outline
    Rect r = MakeRectTLBR(s.p1, s.p2);
    w.DrawRect(r, s.fill);
    w.DrawRect(r, Null, s.pen, s.stroke);
}
bool Rect_Hit(const Shape& s, Point pt) {
    // true if the point lies inside the rect bounds
    return MakeRectTLBR(s.p1, s.p2).Contains(pt);
}

void Circle_Paint(const Shape& s, Draw& w) {
    // radius is stored in p2.x (simple convention for demo)
    int r = max(1, abs(s.p2.x));
    w.DrawEllipse(RectC(s.p1.x-r, s.p1.y-r, 2*r, 2*r), s.stroke, s.pen, s.fill);
}
bool Circle_Hit(const Shape& s, Point pt) {
    // point-in-disc test
    int r=max(1,abs(s.p2.x)); int dx=pt.x-s.p1.x, dy=pt.y-s.p1.y; return dx*dx+dy*dy <= r*r;
}

void Line_Paint(const Shape& s, Draw& w) {
    // draw a simple stroke from p1 to p2
    w.DrawLine(s.p1.x,s.p1.y,s.p2.x,s.p2.y,s.pen,s.stroke);
}
bool Line_Hit(const Shape& s, Point pt) {
    // near-segment with tolerance
    return HitSeg(s.p1,s.p2,pt,max(3,s.pen));
}
```

---

## 4½) Scaling the pattern for **large data** (10³ → 10⁷ items)

> The same Model → Ops → Facade architecture scales if we add **indirection, paging, and caches**. This section shows how to adapt the pattern for *big lists*, *sort/filter models*, and *lazy hydration* (e.g., thumbnails or metadata).

### A) Storage strategies

**1) Packed POD vectors (memory‑resident)**

* Use `Vector<Shape>` (or your type) for up to a few million small items.
* Mark as `Moveable<Shape>` to keep reallocation cheap.
* Keep heavy blobs (images, long strings) **out of the model**; store keys/ids to blob stores.

**2) Indirected pools (stable ids)**

* Assign a stable `ItemId` (`int` or `uint32`), store items in a `Vector<Item>` pool.
* Maintain a `Vector<int> order` for the current view (sorted/filtered) that stores **indices into the pool**.
* Facade becomes `ItemView(pool[order[i]])` — stable id makes cross‑thread jobs and caches trivial.

**3) Paged stores (out‑of‑core)**

* For 10⁷+ items, break data into **blocks** (`N=4096…16384` items each), memory‑map or load on demand.
* Maintain an **LRU block cache** and a **prefetcher** (next/prev pages based on scroll direction).
* Keep only lightweight **headers** resident (id, type, a few ints); hydrate details lazily.

```cpp
struct ItemId { int v=-1; bool IsValid() const { return v>=0; } };

struct ItemHeader { ItemId id; byte type; int a,b; /* light */ };

// Paged store: block → vector of headers + side blobs (mmapped or file-backed)
struct Block { Vector<ItemHeader> hdr; String path; bool dirty=false; };

class PagedStore {
    Vector<String>  block_paths;    // disk paths for blocks
    Vector<Block>   lru;            // small in-memory LRU of hot blocks
    Index<int>      hot_ids;        // map id→lru slot
public:
    const ItemHeader* GetHeader(ItemId id);     // fetch from LRU or load block
    void              PutHeader(ItemHeader h);  // modify; mark dirty
    // Hydration APIs hand back lightweight structs or schedule jobs
};
```

### B) Sort/Filter model (view‑layer, zero copies)

Maintain a **view** over the pool via `order` and optional `mask` for filters. Sorting/filtering permutes `order`; data stays in place.

```cpp
struct SortKey { int64 k1=0; int32 k2=0; byte k3=0; }; // compact composite key

using KeyFn   = SortKey(*)(const Shape&);
using PredFn  = bool(*)(const Shape&);

struct SortModel {
    const Vector<Shape>* pool = nullptr; // not owning
    Vector<int>          order;          // indices into *pool
    Vector<byte>         mask;           // 1=visible, 0=filtered out
    KeyFn                key = nullptr;  // current key extractor
    PredFn               pred = nullptr; // optional filter predicate

    void Reset(const Vector<Shape>& p) {
        pool = &p; order.SetCount(p.GetCount());
        for(int i=0;i<p.GetCount();++i) order[i]=i;
        mask.SetCount(p.GetCount(), 1);
    }
    void ApplyFilter() {
        if(!pred) return; for(int i=0;i<pool->GetCount();++i) mask[i] = pred((*pool)[i]) ? 1:0;
    }
    void Sort() {
        if(!key) return;
        StableSort(order, [&](int a, int b){
            const auto A = key((*pool)[a]);
            const auto B = key((*pool)[b]);
            if(A.k1!=B.k1) return A.k1 < B.k1;
            if(A.k2!=B.k2) return A.k2 < B.k2;
            return A.k3 < B.k3;
        });
    }
};
```

**Where ops help**

* Add optional sort key functions to the ops row when the key is **type‑specific**:

```cpp
// In your ops row for large lists
using KeyFn = SortKey(*)(const Shape&);
struct ShapeOps { PaintFn paint; HitFn hit; NameFn name; KeyFn key_title{}; /* ... */ };
```

Then the view can pick `Ops(s.type).key_title(s)` without switching on `type` at call sites.

### C) UI virtualization (fast lists & grids)

* Render **only what’s visible**. In a grid/list, compute the first/last visible index by scroll offset and row height.
* For thumbnails/expensive paints, draw a **placeholder** first; schedule hydration on a worker pool, then `PostCallback` to repaint cell.
* Keep a small **Image cache** (LRU by ItemId) for hydrated results.

```cpp
struct ThumbCacheEntry { ItemId id; Image img; };
Vector<ThumbCacheEntry> thumb_lru; // smallest workable cache first

// Worker example (CoWork pool)
void QueueHydrate(ItemId id) {
    CoWork::FinLock();
    CoWork().Do([=]{
        Image img = LoadThumbFromDiskOrGen(id);
        // back to GUI thread, update cache safely
        PostCallback([=]{ AddToThumbCache(id, img); /* Refresh cell */ });
    });
}
```

**U++ specifics**

* **Never touch GUI from workers** — wrap GUI updates in `PostCallback` (or `GuiLock` for rare cases; prefer `PostCallback`).
* Use `SetTimeCallback(0, ...)` to coalesce many “hydrated” signals into a single repaint.

### D) Large‑data facades (views over slices)

For big views, construct facades on‑the‑fly for visible items only.

```cpp
// Paint visible rows only (pseudo‑code inside a Ctrl)
for(int row = first; row < last; ++row){
    int idx = sort_model.order[row];
    Shape& s = pool[idx];
    ShapeView(s).Paint(w); // cheap: forwards to ops
}
```

### E) Streaming I/O & persistence

* **Write‑ahead log** (append‑only) for changes + periodic **snapshots** of the pool.
* Prefer **line‑delimited JSON** (NDJSON) or compact binary with version tags.
* On startup: load latest snapshot, replay log. Keep ids stable across sessions.

### F) Error handling & backpressure

* If hydration backlog grows, **drop oldest** or reduce concurrency.
* Add a **retry budget** per item; log persistent failures with a small ring buffer.
* Guard ops pointers with a debug sweep; for prod, add a minimal fallback op row.

---

## 5) End‑to‑end example: big gallery (filesystem + thumbs)

> This stitches the pieces: detector → pool → sort model → virtualized view with lazy hydration.

```cpp
// Model is tiny: path, type, a few ints. Thumbs/EXIF live in side stores.
struct Media { MediaType type{}; String path; String title; int w=0, h=0; int64 mtime=0; };

// Ops row has only what’s truly type‑specific; keys help sorting without switches.
using HydrateFn = void(*)(Media&);    // e.g., read EXIF for Photo, duration for Video
using KeyFn     = SortKey(*)(const Media&);

struct MediaOps { HydrateFn hydrate{}; KeyFn key_title{}; const char* name{}; };

// Facade is again just sugar; call sites remain clear.
class MediaView {
    Media& m;
public:
    explicit MediaView(Media& m) : m(m) {}
    void Hydrate()            { Ops(m.type).hydrate(m); }
    SortKey KeyByTitle()const { return Ops(m.type).key_title ? Ops(m.type).key_title(m) : SortKey{}; }
    String  Label()     const { return m.title; }
};

// Virtual paint (inside Ctrl::Paint): only visible tiles
for(int i = first; i < last; ++i){
    int idx = sort.order[i];
    Media& m = pool[idx];
    if(Image img = FindThumbInCache(m))
        w.DrawImage(cell_rect, img);
    else {
        DrawPlaceholder(w, cell_rect);
        QueueHydrateThumb(m); // worker → cache → Refresh()
    }
}
```

**Why this still fits the pattern**

* The **model** stays POD and small.
* The **ops table** contains only actions that truly differ per type.
* The **facade** keeps usage readable without owning anything.
* The **view** manages large‑data concerns (virtualization, caches, paging) without touching model/ops.

---

## 6) Performance checklist (copy‑paste)

* [ ] Mark large PODs `Moveable<T>`; avoid hidden deep copies.
* [ ] Keep models small; store heavy data via ids to blob stores.
* [ ] Maintain `order` (indices) for sort/filter; avoid shuffling the pool.
* [ ] Virtualize lists/grids; paint only visible range.
* [ ] Lazy‑hydrate expensive fields; LRU cache results.
* [ ] Batch repaints with `SetTimeCallback(0, ...)` or `PostCallback` coalescing.
* [ ] Ensure ops are **pure functions** over model data (easy to test/parallelize).
* [ ] Use `CoWork`/`CoPartition` for CPU‑heavy key extraction.
* [ ] Never update GUI from worker threads; signal back via `PostCallback`.
* [ ] Add a debug sweep that asserts all ops rows are fully populated.

---

## 7) What changes between **small** vs **large** data modes

| Aspect        | Small (≤ 10⁵)               | Large (≥ 10⁶)                                          |
| ------------- | --------------------------- | ------------------------------------------------------ |
| Storage       | Single `Vector<T>`          | Paged store + headers resident                         |
| View          | Direct index                | `order` + `mask` + windowed range                      |
| Sort          | Full `StableSort` on vector | Key precompute + sort `order`; partial/iterative sorts |
| Hydration     | Eager on add                | Lazy, worker pool, LRU caches                          |
| Painting      | All items OK                | Virtualized (only visible)                             |
| Serialization | Single JSON snapshot        | Snapshot + append log                                  |
| Concurrency   | Occasional threads          | Regular worker pool; backpressure policies             |
| IDs/Lifetimes | Optional                    | **Stable ItemId required**                             |

---

## 8) API design rules (keep it simple)

* Keep **ops rows minimal**; only include actions that are truly type‑specific.
* Put **composed convenience** into facades or view code, not ops.
* Prefer **pure functions** (`op(model) → result`) for ops; side effects make parallelization hard.
* Name functions **literally** (`Photo_Hydrate`, `Rect_Paint`, `Media_KeyByTitle`).
* Keep comments **one line right above the function** explaining *what* and *why*.

---

> With these additions, the guide now covers both **small** and **large** data regimes using the same mental model. Start small (POD + ops + facade), then layer in **order/indices, virtualization, caches, and paging** as your dataset grows.

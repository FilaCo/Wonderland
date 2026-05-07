# Alice Query System v1 Specification

## Overview

Alice's query system is designed around three core principles:
- **ECS-native**: Everything is expressed through entities, props, and queries
- **Stateless pipelines**: Systems are declarative pipelines over entity streams
- **Explicit mutations**: All mutations are clearly declared with keywords

## Core Concepts

### Entity ID
Entities are identified by a unique `id` type (internally an integer). Entity IDs are:
- Comparable for equality only: `id1 == id2`
- Can be stored in props: `prop Collision { first: id; second: id }`
- Can be passed to `fetch` operations
- Cannot be used in arithmetic or other operations

### Props
Props define data attached to entities. Three forms:

```alice
// Marker prop (no data)
prop Ball

// Enum prop (variants)
prop Player = First | Second

// Record prop (fields)
prop Position { x: f32; y: f32 }
prop Velocity { dx: f32; dy: f32 }
prop Collision { first: id; second: id; normal: Vec }
```

### Child Entities with Relationships
Entities can have parent-child relationships:

```alice
prop Parent { entity: id }

// Ball as parent
spawn { Ball, Position, Velocity }

// Collider as child
spawn { Collider, Parent(ball_entity) }
```

## Query Syntax

### Named Query Declarations
Queries can be declared as reusable type-like definitions:

```alice
query Moving = Position, Velocity with Alive
query PhysicsBody = Position, Velocity, Collider
```

### Inline Queries in Systems
Queries start a pipeline stream:

```alice
query selector_list [filter_list]
```

**Selectors:**
```alice
query entity, pos: Position, vel: mut Velocity, opt_vel: ?Velocity
```

- `entity` - binds entity ID
- `prop: Type` - binds prop value (read-only)
- `prop: mut Type` - binds prop value (mutable)
- `prop: ?Type` - binds prop value (optional, Option<T>)

**Filters:**
```alice
query Position, Velocity
with Ball          // structural: entity has Ball
without Dead       // structural: entity lacks Dead  
filter vel.dx > 0  // value-based: runtime condition
```

Filters can be chained:
```alice
query Position, Velocity with Ball without Dead filter vel.dx > 0
```

### Query Composition
Queries cannot be directly composed/inherited. Use marker props instead:

```alice
// Use marker props for patterns
prop Alive
prop Moving

// Then filter by markers
query Position, Velocity with Alive, Moving
```

## Pipeline Operators

### derive
Stateless transformation/computation:

```alice
derive padding = 20.
derive half_size = window.size / 2
derive dt = delta_time()
```

Derive creates temporary values available in the pipeline scope.

### fetch
Fetch props from entity by ID (requires entity ID from previous query):

```alice
query collision: Collision
fetch first_pos: Position, first_vel: mut Velocity from collision.first
fetch second_pos: Position, second_vel: ?Velocity from collision.second
```

**Semantics:**
- `fetch` can only be used after a query that provides entity IDs
- Entity ID must come from a queried prop (e.g., `collision.first`)
- Multiple props can be fetched from same entity in one statement
- `?Type` returns `Option<T>` if prop doesn't exist
- Without `?`, fetch fails at runtime if prop missing

**Example:**
```alice
in Update {
    query collision: Collision
    fetch first_pos: Position, first_vel: ?Velocity from collision.first
    derive first_vel = match first_vel {
        Some(v) => v;
        None => Velocity::default!()
    }
}
```

### insert
Add props to existing entities:

```alice
insert entity_id { Prop1, Prop2, ... }
```

**Example:**
```alice
in Update {
    query collision: Collision
    fetch first_vel: mut Velocity from collision.first
    derive first_vel.dx = first_vel.dx * -1
    insert collision.first { first_vel }
}
```

Multiple props:
```alice
insert entity_id { vel, pos }
```

### erase
Remove props from existing entities:

```alice
erase entity_id { Prop1, Prop2, ... }
```

**Example:**
```alice
in Update {
    query entity with Dead
    erase entity { Dead }
}
```

### spawn
Create new entities:

```alice
spawn { Prop1, Prop2, ... }
```

**Example:**
```alice
in Startup {
    spawn { Ball, Position::default!(), Velocity { dx: 1., dy: 0. } }
}
```

Spawn applies to each entity in the current stream (if following a query).

### despawn
Destroy entities:

```alice
despawn { }
```

**Example:**
```alice
in Update {
    query entity; with Ball; filter pos.x > SCREEN_WIDTH; despawn { }
}
```

Despawn applies to each entity in the current stream.

### map (future)
Stateless transformation over query results:

```alice
query Position, Velocity | map pos, vel => pos.x + vel.dx
```

Reserved for future versions.

## Option<T> Builtin

Alice provides a builtin `Option<T>` type for optional values:

```alice
prop Option<T> = Some(T) | None
```

**Usage:**
```alice
derive vel: ?Velocity = fetch!<Velocity>(entity)  // Returns Option<Velocity>

// Match expression to handle Option
derive vel = match opt_vel {
    Some(v) => v;
    None => Velocity::default!()
}
```

## Match Expressions

Pattern matching on Option and enum types:

```alice
match value {
    Pattern1 => expr1;
    Pattern2 => expr2;
}
```

**Examples:**
```alice
// Option handling
derive vel = match opt_vel {
    Some(v) => v;
    None => Velocity::default!()
}

// Enum handling
derive player_name = match player {
    First => "Player 1";
    Second => "Player 2"
}
```

Match is an expression, not a statement. It returns a value.

## Intrinsics

Intrinsics are runtime-provided functions marked with `!` suffix:

### Entity Lifecycle
- `default!()` - Default value for a prop type
- `spawn { ... }` - Create entity (keyword, not intrinsic)
- `despawn { }` - Destroy entity (keyword, not intrinsic)

### Entity Operations
- `insert entity_id { ... }` - Add props (keyword)
- `erase entity_id { ... }` - Remove props (keyword)

### Time
- `delta_time!() -> f32` - Frame delta time
- `time!() -> f32` - Current game time

### Physics
- `physics_step!()` - Run physics simulation (generates Collision entities)

### Math (potential)
- `magnitude!(x: f32, y: f32) -> f32`
- `normalize!(x: f32, y: f32) -> (f32, f32)`
- `lerp!(a: f32, b: f32, t: f32) -> f32`

### Collision (potential)
- `collides!(a: Position, b: Position) -> bool`
- `reflect!(vel: Velocity, normal: Vec)`

**Note:** Many math operations could be Alice-level functions instead of intrinsics.

## System Syntax

Systems are defined with the `in <Stage>` syntax:

```alice
in Startup {
    // Runs once at game start
    spawn { Camera, Transform::default!() }
}

in BeforeUpdate {
    // Runs before Update (collision detection)
    physics_step!()
}

in Update {
    // Main game logic
    query pos: Position, vel: mut Velocity; with Ball; derive {
        pos.x += vel.dx * delta_time!()
    }
}

in AfterUpdate {
    // Cleanup after Update
    query entity; with Collision; despawn { }
}

in FixedUpdate {
    // Runs at fixed intervals (physics)
}
```

### Multiple Systems in Same Stage

```alice
in Update {
    // System 1: movement
    query pos: Position, vel: mut Velocity; with Ball; derive {
        pos.x += vel.dx
    }
    
    // System 2: rendering (runs simultaneously)
    query pos: Position; with Ball; derive {
        draw_circle!(pos.x, pos.y, BALL_RADIUS)
    }
}
```

**Important:** All systems in the same stage execute **simultaneously**. There is no ordering within a stage.

### Built-in Stages

Execution order:
1. `Startup` - Initialization (runs once)
2. `BeforeUpdate` - Pre-processing (collision detection)
3. `Update` - Main logic
4. `AfterUpdate` - Cleanup
5. `FixedUpdate` - Fixed-interval physics

## Collision Pattern

### Collision as Entity

Collision is spawned as a separate entity with entity ID references:

```alice
prop Collision { first: id; second: id; normal: Vec }

// BeforeUpdate: physics spawns Collision entities
in BeforeUpdate {
    physics_step!()  // Detects collisions, spawns Collision entities
}

// Update: handle collisions
in Update {
    query collision: Collision
    fetch first_pos: Position, first_vel: mut Velocity from collision.first
    fetch second_pos: Position, second_vel: mut Velocity from collision.second
    derive {
        reflect!(first_vel, collision.normal)
        reflect!(second_vel, collision.normal)
    }
}

// AfterUpdate: cleanup Collision entities
in AfterUpdate {
    query entity; with Collision; despawn { }
}
```

### Collidable Entities

Entities that can collide must have a `Collider` prop:

```alice
prop Collider { shape: Shape }

// Simple: Collider on main entity
spawn { Ball, Position, Velocity, Collider { shape: Circle(10) } }

// Complex: Collider as child entity
spawn { Ball, Position, Velocity }
spawn { Collider { shape: Circle(10) }, Parent(ball_id) }
```

## Complete Example: Pong Ball Movement

```alice
prop Position { x: f32; y: f32 }
prop Velocity { dx: f32; dy: f32 }
prop Ball
prop Collider { shape: Shape }

// Spawn ball at startup
in Startup {
    spawn {
        Ball,
        Position { x: 0., y: 0. },
        Velocity { dx: 1., dy: 0.5 },
        Collider { shape: Circle(10) }
    }
}

// Move ball every frame
in Update {
    query pos: mut Position, vel: Velocity; with Ball; derive {
        pos.x += vel.dx * delta_time!()
        pos.y += vel.dy * delta_time!()
    }
}

// Handle ball-wall collisions
in Update {
    query collision: Collision
    fetch ball_pos: Position, ball_vel: mut Velocity from collision.first
    derive {
        reflect!(ball_vel, collision.normal)
        ball_vel.dx *= 1.05  // Speed up
    }
    insert collision.first { ball_vel }
}
```

## Grammar Summary (EBNF)

```ebnf
query_decl = "query" ident "=" query_expr .

query_expr = "query" query_selectors [ query_filters ] .

query_selectors = query_selector { "," query_selector } .

query_selector = "entity"
               | ident ":" "mut" type_ref
               | ident ":" "?" type_ref
               | ident ":" type_ref .

query_filters = { query_filter } .

query_filter = "with" type_ref
             | "without" type_ref
             | "filter" expr .

pipeline = { pipeline_stage } .

pipeline_stage = "derive" derive_binding
              | "fetch" fetch_bindings "from" expr
              | "insert" expr "{" prop_list "}"
              | "erase" expr "{" prop_list "}"
              | "spawn" "{" prop_list "}"
              | "despawn" "{" "}" .

derive_binding = ident "=" expr
              | ident ":" type_ref "=" expr .

fetch_bindings = fetch_binding { "," fetch_binding } .

fetch_binding = ident ":" "mut" type_ref
             | ident ":" "?" type_ref
             | ident ":" type_ref .

prop_list = expr { "," expr } .

type_ref = ident [ "::" ident ] .

expr = match_expr
     | binary_expr
     | unary_expr
     | primary_expr .

match_expr = "match" expr "{" match_arm { ";" match_arm } "}" .

match_arm = pattern "=>" expr .

system = "in" stage_name "{" { pipeline } "}" .

stage_name = "Startup"
          | "BeforeUpdate"
          | "Update"
          | "AfterUpdate"
          | "FixedUpdate"
          | ident .  // User-defined stages (future)
```

## Design Rationale

### Why fetch is a keyword, not intrinsic
`fetch` is fundamental to entity-by-ID operations. Making it a keyword:
- Makes the syntax more readable: `fetch vel from entity`
- Emphasizes that it's a core ECS operation
- Avoids the verbose `fetch!<Type>(id)` syntax

### Why insert/erase are keywords
Consistency with `spawn`/`despawn`. All entity/prop mutations use block syntax:
- `spawn { ... }` - create entity
- `despawn { }` - destroy entity
- `insert id { ... }` - add props
- `erase id { ... }` - remove props

### Why Collision is a separate entity
- Pure ECS: everything is entities and props
- No special event system needed
- Collision is queryable like any other data
- Clean lifecycle: spawn in BeforeUpdate, despawn in AfterUpdate
- C interoperability: just entity IDs and props, no events

### Why Option<T> instead of null
- Type-safe: `Option<T>` is explicit in types
- Match expressions force handling of None case
- No null pointer errors
- Aligns with Rust-like safety

### Why fetch requires entity from queried prop
Semantic safety: ensures entity ID is valid and part of the current ECS world. Prevents arbitrary entity lookups that could be invalid or stale.

### Why simultaneous execution within stage
ECS philosophy: systems should be independent and parallelizable. Within a stage, all systems access disjoint data (enforced by mut tracking). Ordering is implicit through stage separation.

## Future Considerations

### Not in v1, but potential future features:
- `map` operator for stateless transformations
- User-defined stages for custom ordering
- Query change detection (`filter prop.changed()`)
- Entity relationship queries beyond Parent
- Spatial partitioning intrinsics for collision
- Query parameterization with generics

## Open Questions for v1

1. **Shape type for Collider** - builtin or user-defined?
2. **Vec type** - builtin or user-defined?
3. **Builtins for rendering** - `draw_circle!()`, `draw_rect!()` - what's the API?
4. **Window/Camera access** - how to query singleton resources?
5. **KeyCode enum** - builtin or user-defined?
6. **Match exhaustiveness** - compiler checks or runtime error?
7. **derive vs let** - is derive only for pipelines, or can it be used standalone?

## Implementation Priority

For v1 implementation:
1. Parser for query syntax
2. Query execution engine (stream-based)
3. Entity ID type and comparison
4. Option<T> builtin
5. Match expression evaluator
6. fetch/insert/erase operations
7. Stage scheduling
8. Basic intrinsics (delta_time, default)

Physics collision detection can be deferred - users can implement simple collision with nested queries for v1.
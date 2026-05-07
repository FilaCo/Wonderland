# Pong in Alice - Query Design Analysis

## Game Entities & Props

```alice
// Props (data definitions)
prop Position {
  x: f32,
  y: f32,
}

prop Velocity {
  dx: f32,
  dy: f32,
}

prop Collider {
  width: f32,
  height: f32,
}

prop Ball;
prop Paddle;
prop Wall;
prop Goal {
  player_id: u32,
}

prop Player {
  id: u32,
}

prop Score {
  value: u32,
}

prop AI {
  difficulty: f32,
}
```

## Systems Design

### System 1: Move Ball
```alice
system move_ball {
  for pos, vel in query Position, mut Velocity, Ball {
    pos.x += vel.dx;
    pos.y += vel.dy;
  }
}
```

### System 2: Ball-Wall Collision
```alice
system ball_wall_collision {
  for ball_pos, ball_vel in query Position, mut Velocity, Ball {
    for wall_pos, wall_collider in query Position, Collider, Wall {
      // BUILTIN NEEDED: collision detection
      if collides(ball_pos, ball_vel, wall_pos, wall_collider) {
        // BUILTIN NEEDED: collision response
        reflect(ball_vel, wall_collider);
      }
    }
  }
}
```

### System 3: Ball-Paddle Collision
```alice
system ball_paddle_collision {
  for ball_pos, ball_vel in query Position, mut Velocity, Ball {
    for paddle_pos, paddle_collider in query Position, Collider, Paddle {
      if collides(ball_pos, ball_vel, paddle_pos, paddle_collider) {
        reflect(ball_vel, paddle_collider);
        // Increase ball speed slightly
        ball_vel.dx *= 1.05;
        ball_vel.dy *= 1.05;
      }
    }
  }
}
```

### System 4: Score Goals
```alice
system score_goals {
  for ball_pos, ball_vel in query Position, mut Velocity, Ball {
    for goal_pos, goal in query Position, Goal {
      if collides(ball_pos, ball_vel, goal_pos, ...) {
        // Update score
        for score in query mut Score, Player filter Player.id == goal.player_id {
          score.value += 1;
        }
        // Reset ball position
        ball_pos.x = 0.0;
        ball_pos.y = 0.0;
        // Reverse direction
        ball_vel.dx *= -1.0;
      }
    }
  }
}
```

### System 5: Player Input
```alice
system player_input {
  for pos in query mut Position, Paddle, Player {
    // BUILTIN NEEDED: input handling
    if input_key_pressed(KeyCode.Up) {
      pos.y -= speed;
    }
    if input_key_pressed(KeyCode.Down) {
      pos.y += speed;
    }
  }
}
```

### System 6: AI Control
```alice
system ai_control {
  for paddle_pos in query mut Position, Paddle, AI {
    for ball_pos in query Position, Ball {
      // BUILTIN NEEDED: AI logic helpers
      let target_y = predict_ball_y(ball_pos, paddle_pos);
      paddle_pos.y = lerp(paddle_pos.y, target_y, ai.difficulty);
    }
  }
}
```

### System 7: Render
```alice
system render {
  for pos, collider in query Position, Collider {
    // BUILTIN NEEDED: rendering
    draw_rect(pos.x, pos.y, collider.width, collider.height);
  }
  
  for pos in query Position, Ball {
    draw_circle(pos.x, pos.y, BALL_RADIUS);
  }
}
```

## Analysis: Intrinsics vs Composable

### Required Intrinsics (Runtime Builtins)

#### Math & Physics
```alice
// Collision detection
intrinsic collides(pos1: Position, vel1: Velocity, pos2: Position, col2: Collider) -> bool;

// Collision response
intrinsic reflect(vel: mut Velocity, collider: Collider);

// Math
intrinsic lerp(a: f32, b: f32, t: f32) -> f32;
intrinsic magnitude(x: f32, y: f32) -> f32;
intrinsic normalize(x: f32, y: f32) -> (f32, f32);

// Ball prediction (for AI)
intrinsic predict_ball_y(ball_pos: Position, paddle_pos: Position) -> f32;
```

#### Input
```alice
intrinsic input_key_pressed(key: KeyCode) -> bool;
intrinsic input_key_down(key: KeyCode) -> bool;
intrinsic input_key_up(key: KeyCode) -> bool;
```

#### Rendering
```alice
intrinsic draw_rect(x: f32, y: f32, w: f32, h: f32);
intrinsic draw_circle(x: f32, y: f32, r: f32);
intrinsic draw_text(text: string, x: f32, y: f32);
```

#### Entity Lifecycle
```alice
intrinsic spawn(props: ...) -> entity;
intrinsic despawn(entity: entity);
```

#### Time
```alice
intrinsic delta_time() -> f32;
intrinsic time() -> f32;
```

### Query Fragments (Reusable Patterns)

```alice
// Structural patterns
query Ball = with Ball;
query Paddle = with Paddle;
query Player = with Player;
query AI = with AI;

// Behavioral patterns  
query Moving = Position, Velocity filter magnitude(Velocity.dx, Velocity.dy) > 0;
query Collidable = Position, Collider;

// Composed patterns
query BallPhysics = Position, mut Velocity, Ball;
query PaddleControl = mut Position, Paddle, Player;
query AIControl = mut Position, Paddle, AI;
```

### Composable Systems

Systems can be composed from query fragments:

```alice
// Base query fragments
query Ball = with Ball;
query Paddle = with Paddle;
query Moving = Position, Velocity;

// Combined
query MovingBall = Moving, Ball;

// System uses combined query
system move_ball {
  use MovingBall;  // or inline: query Position, mut Velocity, Ball
  for pos, vel in MovingBall {
    pos.x += vel.dx * delta_time();
    pos.y += vel.dy * delta_time();
  }
}
```

## Key Insights

### 1. Intrinsics Should Be Minimal
Only provide what can't be expressed in Alice:
- Platform I/O (input, rendering, audio)
- Hardware access (time, random)
- Complex physics (collision, raycasting)
- Entity lifecycle (spawn, despawn)

### 2. Queries Enable Composition
```alice
// Define once, use many times
query Moving = Position, mut Velocity;
query PhysicsBody = Position, Velocity, Collider;

// Compose
query BallPhysics = Moving, Ball;
query PaddlePhysics = PhysicsBody, Paddle;
```

### 3. Systems Are the "Program"
- Systems combine queries + intrinsics
- Systems define the schedule/execution order
- Systems can compose queries

### 4. No Need for Complex Metaprogramming (Yet)
For Pong-level complexity:
- Named query fragments are sufficient
- Simple composition with commas
- Filters for value-based queries

### 5. Filter Patterns Emerge
```alice
// Common filter patterns become clear
query FastMoving = Position, Velocity filter magnitude(Velocity.dx, Velocity.dy) > 10;
query InBounds = Position filter Position.x > 0 && Position.x < SCREEN_WIDTH;
query ControlledBy(id: u32) = Player filter Player.id == id;  // Need parameterization?
```

## Questions Arising from Pong

### 1. Query Parameterization
Should queries support parameters?

```alice
// Option A: Generic queries
query ControlledBy = Player filter Player.id == $id;

// Option B: Function-like
fn query_controlled_by(id: u32) -> Query<Player> {
  query Player filter Player.id == id
}

// Option C: Just inline it
for player in query Player filter Player.id == player_id { ... }
```

**Recommendation for v1**: Option C (inline). Simpler, defer parameterization.

### 2. Nested Queries
Nested queries in systems (like collision detection):

```alice
// Current design
system ball_wall_collision {
  for ball_pos, ball_vel in query Position, mut Velocity, Ball {
    for wall_pos, wall_collider in query Position, Collider, Wall {
      if collides(...) { ... }
    }
  }
}
```

Is this the right pattern, or should we have:
```alice
// Query products?
for ball, wall in query (Position, mut Velocity, Ball), (Position, Collider, Wall) {
  ...
}
```

**Recommendation for v1**: Nested queries are fine for v1. Query products are an optimization for later.

### 3. Entity References
Should we be able to store entity IDs and query them later?

```alice
prop Ball {
  entity: entity,  // Store reference to another entity
}

// Query by entity ID?
for ball in query Ball filter ball.entity == some_id { ... }
```

**Recommendation for v1**: Yes, entity IDs should be comparable. Defer complex entity relationship queries.

### 4. Query Change Detection
For optimization, do we need:

```alice
// Only run if Velocity changed
query Position, mut Velocity filter Velocity.changed();

// Or as a builtin?
if query_changed(Position, Velocity) {
  // Expensive computation
}
```

**Recommendation for v1**: Defer change detection. It's an optimization that can be added later.

### 5. System Scheduling
How do systems define execution order?

```alice
// Explicit ordering?
system move_ball before ball_collision;
system ball_collision after move_ball, before render;

// Or phases?
system move_ball in phase Physics;
system ball_collision in phase Physics after move_ball;
system render in phase Render;
```

**Recommendation for v1**: Explicit ordering with `before`/`after`. Simple and clear.

## Proposed Query v1 Specification

Based on Pong analysis:

### Grammar

```ebnf
query_decl = "query" ident "=" query_expr [ query_filters ] .

query_expr = "query" query_selectors .

query_selectors = query_selector { "," query_selector } .

query_selector = "entity"
               | "mut" prop_ref
               | "?" prop_ref
               | prop_ref .

query_filters = { query_filter } .

query_filter = "with" prop_ref 
             | "without" prop_ref 
             | "filter" expr .

prop_ref = ident [ "::" ident ] .
```

### Intrinsics Needed for v1

**Essential:**
- `spawn()` / `despawn()` - entity lifecycle
- `delta_time()` - frame timing
- Basic rendering (draw_rect, draw_circle)
- Basic input (key_pressed)

**Math (can be Alice-level or intrinsic):**
- `magnitude(x, y)`
- `normalize(x, y)`
- `lerp(a, b, t)`
- `collides(...)` - or implement in Alice

**Optional for v1:**
- Change detection (`changed()`)
- Entity ID comparisons
- Random numbers

### Query Fragment Patterns

```alice
// Simple markers
query Ball = with Ball;
query Paddle = with Paddle;

// With behavior
query Moving = Position, Velocity filter magnitude(Velocity.dx, Velocity.dy) > 0;

// Composed
query BallPhysics = Position, mut Velocity, Ball;
query BallAndWalls = query BallPhysics, query (Position, Collider, Wall);
```

Wait, that last one is awkward. Better to keep queries flat for v1:

```alice
// Just write it out
query BallPhysics = Position, mut Velocity, Ball;
query WallCollider = Position, Collider, Wall;

// In system, iterate both separately
system collision {
  for ball_pos, ball_vel in BallPhysics {
    for wall_pos, wall_col in WallCollider {
      ...
    }
  }
}
```

## Next Steps

1. **Confirm design decisions** with user
2. **Write formal spec** for queries
3. **Define intrinsic list** for v1
4. **Implement parser** for query syntax
5. **Implement runtime** query execution
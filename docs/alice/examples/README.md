# Alice — design examples

These are *aspirational* `.alice` programs used to drive language design. They are not yet parseable; the grammar in [../spec.md](../spec.md) does not (yet) support several of the constructs that appear here.

Treat the syntax as a proposal — changing an example is cheaper than changing the spec.

| File | Demonstrates |
| --- | --- |
| [01_types.alice](01_types.alice) | The four `prop` shapes: record, tuple, unit, enum. |
| [02_movement.alice](02_movement.alice) | Per-frame iteration; mutating selected bindings. |
| [03_bounds_despawn.alice](03_bounds_despawn.alice) | Filter expressions; `despawn` semantics. |
| [04_collision.alice](04_collision.alice) | Entity pairs, collision detection, relationship queries. |

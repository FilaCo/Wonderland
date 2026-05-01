# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

```sh
cargo build                  # build all crates
cargo test --verbose         # run all tests
cargo test --lib             # library tests only
cargo test -p <crate>        # test a single crate
cargo clippy                 # lint (enforced pre-push)
cargo fmt                    # format (checked pre-commit)
```

Git hooks (Husky) enforce `cargo fmt --check` on commit and `cargo clippy` + `cargo test --verbose` on push. Conventional commits are required (commitlint).

## Architecture

Wonderland is a game engine. It has two major components:

### Alice DSL compiler (`alice/`)

A compiler for the Alice scripting language — an ECS-oriented DSL for game logic. The pipeline:

```text
CLI / REPL (ac)
  → SourceFile (ac_ir)           raw input wrapper
  → parse_file_query (ac_query)  lex + parse, backed by Salsa
  → AliceFile AST (ac_ir)        top-level statements + expressions
  → Diagnostics (ac_diag)        span-tracked errors/warnings
  → Ariadne output               pretty error rendering
```

**Crates:**

- `ac` — CLI entry point and REPL driver ([alice/ac/src/driver.rs](alice/ac/src/driver.rs))
- `ac_parser` — hand-written lexer and parser (`memchr`, `unicode-xid`)
- `ac_ir` — tokens, AST nodes, `SourceFile`, `Span`
- `ac_db` — Salsa database trait
- `ac_query` — Salsa-tracked queries (file parsing, evaluation)
- `ac_interface` — `AliceDatabase` implementation; public API surface
- `ac_diag` — `Diagnostic` type with Error/Warning/Advice severity

The Alice language has three primitives: **Ids** (opaque entity handles), **Props** (component data), and **Systems** (query + mutation logic). The grammar is specified in [docs/alice/spec.md](docs/alice/spec.md).

### ECS runtime (`engine/wonderland_ecs/`)

A minimal Entity-Component-System engine:

- `World` — spawns entities, owns component pools
- `Id` — 32-bit entity identifier (20-bit position, 12-bit version)
- `Prop` — marker trait for components
- `Pool<T>` — dense component storage

## Key workspace dependencies

| Crate | Role |
| --- | --- |
| `salsa 0.26` | Incremental compilation database backing `ac_query` |
| `ariadne 0.6` | Pretty-printed diagnostics in the CLI |
| `clap 4` | CLI argument parsing in `ac` |
| `insta` / `expect-test` | Snapshot and expectation testing (not yet widely used) |
| `thiserror 2` | Error type derivation |

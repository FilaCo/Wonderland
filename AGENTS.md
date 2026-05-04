# AGENTS.md

## Commands

```sh
cargo build                  # build all crates
cargo test --verbose         # run all tests
cargo test -p <crate>        # test single crate (e.g., cargo test -p ac_parser)
cargo clippy                 # lint (enforced pre-push)
cargo fmt                    # format (checked pre-commit)
```

## Git Hooks

Husky enforces on commit: `cargo fmt --check`
Husky enforces on push: `cargo clippy && cargo test --verbose`
Commit messages must follow [conventional commits](https://www.conventionalcommits.org/).

## Workspace Structure

```
alice/           # Alice DSL compiler
  ac/            # CLI entrypoint (binary: `ac`)
  ac_parser/     # Lexer + parser
  ac_ir/         # AST, tokens, SourceFile, Span
  ac_query/      # Salsa queries
  ac_db/         # Salsa database trait
  ac_interface/  # Public API surface
  ac_diag/       # Diagnostic types
engine/
  wonderland_ecs/  # ECS runtime (World, Id, Prop, Pool)
```

## Key Files

- `alice/ac/src/driver.rs` — CLI/REPL entry point
- `docs/alice/spec.md` — Alice language grammar specification

## Notes

- Rust edition 2024
- Tests are inline (`#[cfg(test)]` modules), not in `/tests` directories
# Debugging Settings

`/cg_term_project/assets/debug.json` 을 만들고 다음 내용을 붙여넣는다.

```json
{
  "$schema": "./debug_schema.json",
  "features": ["show_hitbox", "enable_stage_navigation", "show_coords"]
}
```

`"features"`를 수정하면 필요한 기능만 선택적으로 활성화 가능하다.

- `"show_hitbox"`: 엔티티의 박스와 이름을 표시한다.
- `"enable_stage_navigation"`: 숫자 [1]을 눌러서 다음 스테이지로 이동할수 있게 한다.
- `"show_coords"`: 현재 좌표를 표시한다.

# See Also

- [Project Anatomy](./anatomy.md): 프로젝트에 대한 개괄적인 해설이다.

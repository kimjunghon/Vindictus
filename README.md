# Vindictus

- 플레이 영상

[https://youtu.be/J-mNhm9mx3k](https://youtu.be/J-mNhm9mx3k)

- 프로젝트 개요
    - C++과 DirectX11 기반으로 액션 중심 게임 시스템 구현을 목표로 제작한
    마비노기 영웅전 모작 프로젝트입니다.
    - 플레이어 시스템과 몬스터 AI를  핵심으로 다양한 게임 플레이 기능을 구현했습니다.

- 개발 목표
    - 원작 게임의 전투 및 캐릭터 시스템을 분석하여 
    Controller + FSM + Animation State Machine 기반의 플레이어 구조 구현
    - 행동 트리를 이용한 몬스터 AI 시스템 구현
    - 선형 보간 및 Catmull-Rom Spline 보간을 적용한 컷씬 카메라 시스템 구현
    - NavMesh 기반 이동 영역 제한과 
    Collider Channel + Broad Phase를 활용한 충돌 처리 구조 구현
    - 몬스터, NavMesh, 이펙트, 카메라, Animation 데이터 입출력을 위한 ImGui 기반 툴 구현
    
- 구현 목록

| **구분** | **내용** |
| --- | --- |
| 플레이어 | 이동 / 스태미너 <br> 콤보 시스템 / 방어 / 반격 / 회피 <br> 장착 장비에 따른 스테이터스 변화 |
| 몬스터 | 행동 트리 기반 AI |
| 애니메이션 | Skinning 기반 3D 애니메이션 <br> 루트 모션 / 애니메이션 블렌딩 |
| 투사체 | Catmull-Rom Spline 기반 유도형 투사체 <br> Bezier Curve 기반 곡선 투사체 |
| 전투 상호작용 | 플레이어 장비 파괴 / 몬스터 부위 파괴 <br> 공격 강도(약 / 중 / 강)에 따른 경직 및 넉백  |
| 카메라 | 타겟 카메라 / 컷씬 카메라 |
| UI | 플레이어 HP / 스태미너 / 보스 HP <br> 인벤토리 / 장비 슬롯 <br> NPC 대화 / 염색 <br> 전투 입장 / 로딩 |
| 캐릭터 커스터마이징 | 장착 장비에 따른 캐릭터 외형 변경 <br> 장비 파츠 별 염색  |
| NPC | 대화 NPC / 전투 입장 NPC <br> 플레이어를 따라다니는 NPC |
| 충돌 | AABB / OBB <br> Collision Channel 기반 충돌 필터링 <br> Broad Phase 충돌 검사 <br> NavMesh 기반 이동 가능 영역 구축 |
| 이펙트 | Sprite 기반 2D 이펙트 / Billboard 이펙트 <br> Catmull-Rom Spline 기반 Trail 이펙트 <br> 파티클 이펙트 |
| 렌더링 & 그래픽스 | 렌더 타겟 기반 렌더링 파이프라인 <br> 조명 및 그림자 표현 <br><br> **Post Processing** <br> Bloom / Emissive / 화면 왜곡 |
| 툴 | 맵 오브젝트 배치 툴 <br> NavMesh 생성 및 편집 툴  <br> 애니메이션 데이터 생성 및 편집 툴 <br> 몬스터 배치 툴 <br> 컷씬 카메라 데이터 편집 툴 <br> 이펙트 생성 및 편집 |

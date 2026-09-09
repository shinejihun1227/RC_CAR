"""
RC_CAR - Chassis Frame Only Builder for Fusion 360

This is the first mechanical model to run.  It creates only the chassis
structure.  No motors, wheels, ESP32, drivers, battery models, or STL
envelopes are inserted into the visible design.

Design basis
------------
The frame combines patterns seen in open robot chassis projects:
  - a rigid lower plate with a controlled lightening opening
  - separate side rails and front/rear bumpers
  - transverse mounting bridges for replaceable motor brackets
  - a universal M3 mounting pattern
  - four upper-deck mounting posts

The nominal chassis is 300 x 400 mm.  It is intentionally a clean mechanical
base that can be checked first.  Motor and electronics mounts are added in a
later assembly script after the chassis dimensions are accepted.
"""

import adsk.core
import adsk.fusion
import traceback


MM_TO_CM = 0.1


def p3(x_mm, y_mm, z_mm=0.0):
    return adsk.core.Point3D.create(
        x_mm * MM_TO_CM,
        y_mm * MM_TO_CM,
        z_mm * MM_TO_CM,
    )


def v3(x_mm, y_mm, z_mm=0.0):
    return adsk.core.Vector3D.create(
        x_mm * MM_TO_CM,
        y_mm * MM_TO_CM,
        z_mm * MM_TO_CM,
    )


def set_visibility(occurrence, visible):
    try:
        occurrence.isLightBulbOn = visible
    except:
        pass


def set_grounded(occurrence, grounded):
    try:
        occurrence.isGrounded = grounded
    except:
        pass


def new_component(parent_component, name, x_mm=0.0, y_mm=0.0,
                  z_mm=0.0, visible=True, grounded=False):
    transform = adsk.core.Matrix3D.create()
    transform.translation = v3(x_mm, y_mm, z_mm)
    occurrence = parent_component.occurrences.addNewComponent(transform)
    if not occurrence:
        raise RuntimeError("Occurrence 생성 실패: {}".format(name))
    occurrence.component.name = name
    set_visibility(occurrence, visible)
    set_grounded(occurrence, grounded)
    return occurrence, occurrence.component


def ensure_parameter(design, name, expression, units, comment):
    existing = design.userParameters.itemByName(name)
    if existing:
        return existing
    return design.userParameters.add(
        name,
        adsk.core.ValueInput.createByString(expression),
        units,
        comment,
    )


def add_parameters(design):
    parameters = [
        ("chassis_width", "300 mm", "mm", "대형 샤시 좌우 폭"),
        ("chassis_length", "400 mm", "mm", "대형 샤시 앞뒤 길이"),
        ("base_thickness", "5 mm", "mm", "하부 플레이트 두께"),
        ("central_opening_width", "140 mm", "mm", "중앙 경량화 개구부 폭"),
        ("central_opening_length", "240 mm", "mm", "중앙 경량화 개구부 길이"),
        ("corner_radius", "12 mm", "mm", "외곽 수직 모서리 라운드"),
        ("m3_clearance_diameter", "3.4 mm", "mm", "M3 통과 구멍 지름"),
        ("side_rail_thickness", "8 mm", "mm", "좌우 레일 두께"),
        ("side_rail_height", "24 mm", "mm", "좌우 레일 높이"),
        ("bumper_width", "270 mm", "mm", "앞뒤 범퍼 폭"),
        ("bumper_thickness", "12 mm", "mm", "앞뒤 범퍼 두께"),
        ("bumper_height", "28 mm", "mm", "앞뒤 범퍼 높이"),
        ("bridge_width", "chassis_width - 40 mm", "mm", "가로 장착 브리지 폭"),
        ("bridge_length", "8 mm", "mm", "가로 장착 브리지 길이"),
        ("bridge_height", "10 mm", "mm", "가로 장착 브리지 높이"),
        ("deck_post_diameter", "10 mm", "mm", "상부 데크 포스트 외경"),
        ("deck_post_height", "22 mm", "mm", "상부 데크 포스트 높이"),
        ("deck_post_hole_x", "95 mm", "mm", "상부 데크 포스트 X 위치"),
        ("deck_post_front_y", "-130 mm", "mm", "상부 데크 전방 포스트 Y 위치"),
        ("deck_post_rear_y", "130 mm", "mm", "상부 데크 후방 포스트 Y 위치"),
    ]
    for name, expression, units, comment in parameters:
        ensure_parameter(design, name, expression, units, comment)


def rectangle_sketch(component, name, width_expr, length_expr,
                     initial_width_mm, initial_length_mm):
    sketch = component.sketches.add(component.xYConstructionPlane)
    sketch.name = name + "_Sketch"
    half_w = initial_width_mm / 2.0
    half_l = initial_length_mm / 2.0
    lines = sketch.sketchCurves.sketchLines.addTwoPointRectangle(
        p3(-half_w, -half_l),
        p3(half_w, half_l),
    )
    width_dim = sketch.sketchDimensions.addDistanceDimension(
        lines.item(0).startSketchPoint,
        lines.item(0).endSketchPoint,
        adsk.fusion.DimensionOrientations.HorizontalDimensionOrientation,
        p3(0, -half_l - 10),
    )
    width_dim.parameter.expression = width_expr
    length_dim = sketch.sketchDimensions.addDistanceDimension(
        lines.item(1).startSketchPoint,
        lines.item(1).endSketchPoint,
        adsk.fusion.DimensionOrientations.VerticalDimensionOrientation,
        p3(half_w + 10, 0),
    )
    length_dim.parameter.expression = length_expr
    return sketch


def circle_sketch(component, name, diameter_expr, initial_diameter_mm,
                  x_mm=0.0, y_mm=0.0, plane=None):
    sketch = component.sketches.add(
        plane if plane else component.xYConstructionPlane
    )
    sketch.name = name + "_Sketch"
    circle = sketch.sketchCurves.sketchCircles.addByCenterRadius(
        p3(x_mm, y_mm),
        (initial_diameter_mm / 2.0) * MM_TO_CM,
    )
    diameter_dim = sketch.sketchDimensions.addDiameterDimension(
        circle,
        p3(x_mm + initial_diameter_mm / 2.0, y_mm),
    )
    diameter_dim.parameter.expression = diameter_expr
    return sketch


def extrude(component, sketch, distance_expr, operation, name,
            symmetric=False):
    if sketch.profiles.count == 0:
        raise RuntimeError("프로파일을 찾을 수 없습니다: {}".format(sketch.name))
    input_def = component.features.extrudeFeatures.createInput(
        sketch.profiles.item(0),
        operation,
    )
    input_def.setDistanceExtent(
        symmetric,
        adsk.core.ValueInput.createByString(distance_expr),
    )
    feature = component.features.extrudeFeatures.add(input_def)
    feature.name = name
    if feature.bodies.count > 0:
        feature.bodies.item(0).name = name + "_Body"
    sketch.isVisible = False
    return feature


def add_box(component, name, width_expr, length_expr, height_expr,
            initial_width_mm, initial_length_mm, initial_height_mm):
    sketch = rectangle_sketch(
        component,
        name,
        width_expr,
        length_expr,
        initial_width_mm,
        initial_length_mm,
    )
    return extrude(
        component,
        sketch,
        height_expr,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        name + "_Extrude",
    )


def make_box_part(parent_component, name, width_expr, length_expr,
                  height_expr, initial_width_mm, initial_length_mm,
                  initial_height_mm, x_mm=0.0, y_mm=0.0, z_mm=0.0,
                  grounded=True):
    occurrence, component = new_component(
        parent_component,
        name,
        x_mm,
        y_mm,
        z_mm,
        True,
        grounded,
    )
    add_box(
        component,
        name,
        width_expr,
        length_expr,
        height_expr,
        initial_width_mm,
        initial_length_mm,
        initial_height_mm,
    )
    return occurrence, component


def cut_rectangle(component, name, width_expr, length_expr,
                  initial_width_mm, initial_length_mm, depth_expr):
    sketch = rectangle_sketch(
        component,
        name,
        width_expr,
        length_expr,
        initial_width_mm,
        initial_length_mm,
    )
    return extrude(
        component,
        sketch,
        depth_expr,
        adsk.fusion.FeatureOperations.CutFeatureOperation,
        name + "_Cut",
    )


def cut_one_circle(component, name, x_mm, y_mm, diameter_expr,
                   initial_diameter_mm, depth_expr):
    sketch = circle_sketch(
        component,
        name,
        diameter_expr,
        initial_diameter_mm,
        x_mm,
        y_mm,
    )
    return extrude(
        component,
        sketch,
        depth_expr,
        adsk.fusion.FeatureOperations.CutFeatureOperation,
        name + "_Cut",
    )


def cut_circles(component, name, points, depth_expr):
    last_feature = None
    for index, (x_mm, y_mm) in enumerate(points, start=1):
        last_feature = cut_one_circle(
            component,
            "{}_{}".format(name, index),
            x_mm,
            y_mm,
            "m3_clearance_diameter",
            3.4,
            depth_expr,
        )
    return last_feature


def add_outer_corner_fillet(component):
    # Cosmetic/strength feature.  If a Fusion release rejects an edge set,
    # leave the valid base plate intact instead of failing the complete build.
    try:
        body = component.bRepBodies.item(0)
        edges = adsk.core.ObjectCollection.create()
        for edge in body.edges:
            start = edge.startVertex.geometry
            end = edge.endVertex.geometry
            if (
                abs(start.x - end.x) < 1e-6
                and abs(start.y - end.y) < 1e-6
                and abs(start.z - end.z) > 1e-4
            ):
                edges.add(edge)
        if edges.count == 0:
            return None
        fillet_input = component.features.filletFeatures.createInput()
        fillet_input.addConstantRadiusEdgeSet(
            edges,
            adsk.core.ValueInput.createByString("corner_radius"),
            True,
        )
        feature = component.features.filletFeatures.add(fillet_input)
        feature.name = "01_Outer_Corner_Fillet"
        return feature
    except:
        return None


def make_base_plate(assembly):
    occurrence, component = make_box_part(
        assembly,
        "01_CHASSIS_Base_Plate",
        "chassis_width",
        "chassis_length",
        "base_thickness",
        300,
        400,
        5,
        0,
        0,
        0,
        True,
    )
    cut_rectangle(
        component,
        "01_Center_Lightening_Opening",
        "central_opening_width",
        "central_opening_length",
        140,
        240,
        "base_thickness",
    )

    # Holes on the outer ring remain supported after the center opening is
    # changed.  The positions are the initial layout for later brackets.
    ring_holes = [
        (-120, -180), (-60, -180), (0, -180), (60, -180), (120, -180),
        (-120, 180), (-60, 180), (0, 180), (60, 180), (120, 180),
        (-135, -100), (-135, 0), (-135, 100),
        (135, -100), (135, 0), (135, 100),
        (-105, -150), (105, -150), (-105, 150), (105, 150),
    ]
    cut_circles(
        component,
        "01_CHASSIS_M3_Perimeter_Hole",
        ring_holes,
        "base_thickness",
    )
    add_outer_corner_fillet(component)
    return occurrence


def make_side_rails(assembly):
    parts = []
    for name, x_mm in (
        ("01_CHASSIS_Left_Side_Rail", -146),
        ("01_CHASSIS_Right_Side_Rail", 146),
    ):
        occurrence, _ = make_box_part(
            assembly,
            name,
            "side_rail_thickness",
            "chassis_length - 20 mm",
            "side_rail_height",
            8,
            380,
            24,
            x_mm,
            0,
            5,
            True,
        )
        parts.append(occurrence)
    return parts


def make_bumpers(assembly):
    parts = []
    for name, y_mm in (
        ("01_CHASSIS_Front_Bumper", 194),
        ("01_CHASSIS_Rear_Bumper", -194),
    ):
        occurrence, _ = make_box_part(
            assembly,
            name,
            "bumper_width",
            "bumper_thickness",
            "bumper_height",
            270,
            12,
            28,
            0,
            y_mm,
            5,
            True,
        )
        parts.append(occurrence)
    return parts


def make_mounting_bridge(assembly, label, y_mm):
    occurrence, component = make_box_part(
        assembly,
        "02_CHASSIS_Mounting_Bridge_" + label,
        "bridge_width",
        "bridge_length",
        "bridge_height",
        260,
        8,
        10,
        0,
        y_mm,
        5,
        True,
    )
    cut_circles(
        component,
        "02_Bridge_M3_Hole_" + label,
        [
            (-120, -2), (-60, -2), (60, -2), (120, -2),
            (-120, 2), (-60, 2), (60, 2), (120, 2),
        ],
        "bridge_height",
    )
    return occurrence


def make_deck_posts(assembly):
    posts = []
    for label, x_mm, y_expr, y_initial in (
        ("FL", -95, "deck_post_front_y", -130),
        ("FR", 95, "deck_post_front_y", -130),
        ("RL", -95, "deck_post_rear_y", 130),
        ("RR", 95, "deck_post_rear_y", 130),
    ):
        occurrence, component = new_component(
            assembly,
            "03_CHASSIS_Upper_Deck_Post_" + label,
            x_mm,
            y_initial,
            5,
            True,
            True,
        )
        sketch = circle_sketch(
            component,
            "03_Deck_Post_Outer_" + label,
            "deck_post_diameter",
            10,
        )
        extrude(
            component,
            sketch,
            "deck_post_height",
            adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
            "03_Deck_Post_Outer_" + label + "_Extrude",
        )
        cut_one_circle(
            component,
            "03_Deck_Post_M3_" + label,
            0,
            0,
            "m3_clearance_diameter",
            3.4,
            "deck_post_height",
        )
        posts.append(occurrence)
    return posts


def add_static_rigid_group(assembly, occurrences):
    try:
        collection = adsk.core.ObjectCollection.create()
        for occurrence in occurrences:
            collection.add(occurrence)
        group = assembly.rigidGroups.add(collection, False)
        group.name = "RG_CHASSIS_Static_Frame"
        return group
    except:
        return None


def build_chassis(design):
    root = design.rootComponent
    add_parameters(design)
    assembly_occurrence, assembly = new_component(
        root,
        "RC_CAR_CHASSIS_ONLY",
        0,
        0,
        0,
        True,
        True,
    )
    structural = [make_base_plate(assembly)]
    structural.extend(make_side_rails(assembly))
    structural.extend(make_bumpers(assembly))
    structural.extend([
        make_mounting_bridge(assembly, "Front", 150),
        make_mounting_bridge(assembly, "Center", 0),
        make_mounting_bridge(assembly, "Rear", -150),
    ])
    structural.extend(make_deck_posts(assembly))
    add_static_rigid_group(assembly, structural)
    return assembly_occurrence


def run(context):
    app = None
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        design = adsk.fusion.Design.cast(app.activeProduct)
        if not design:
            raise RuntimeError("Fusion Design 환경을 열 수 없습니다.")
        build_chassis(design)
        try:
            app.activeViewport.fit()
        except:
            pass
        ui.messageBox(
            "RC_CAR 샤시 프레임만 생성했습니다.\n\n"
            "표시: 하부 플레이트, 좌우 레일, 앞뒤 범퍼, 장착 브리지, 상부 데크 포스트\n"
            "미포함: 모터, 바퀴, ESP32, TB6612FNG, 배터리, STL 부품 모형\n\n"
            "Modify → Change Parameters에서 샤시 크기를 먼저 확인하세요."
        )
    except:
        if ui:
            ui.messageBox(
                "RC_CAR 샤시 생성 실패:\n{}".format(
                    traceback.format_exc()
                )
            )

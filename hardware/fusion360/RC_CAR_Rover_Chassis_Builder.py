"""
RC_CAR - Rover-style chassis builder for Fusion 360

This is a chassis-only model inspired by the proportions and layered layout
of the MakerWorld "RC Rover with Robot Arm 6 DOF" project.  It is a native
Fusion 360 parametric rebuild, not a copy of the MakerWorld STL.

Visible geometry is limited to:
  - a lower tub plate with a lightening/service opening
  - raised side rails and front/rear bumper beams
  - four universal motor-adapter pads for N20/TT adapter plates
  - a raised upper electronics deck
  - a circular arm-base interface on the upper deck

No motor, wheel, servo, controller, battery, or imported STL is created.
The motor adapter pads are intentionally structural interfaces: the exact
N20 encoder or TT mounting bracket is added only after this chassis is
accepted.
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


def safe_set(obj, attr, value):
    try:
        setattr(obj, attr, value)
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
    safe_set(occurrence, "isLightBulbOn", visible)
    safe_set(occurrence, "isGrounded", grounded)
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
        ("rover_width", "320 mm", "mm", "대형 로버 차체 전체 폭"),
        ("rover_length", "420 mm", "mm", "대형 로버 차체 전체 길이"),
        ("base_thickness", "6 mm", "mm", "하부 차체판 두께"),
        ("service_opening_width", "150 mm", "mm", "하부 정비 및 배선 개구부 폭"),
        ("service_opening_length", "260 mm", "mm", "하부 정비 및 배선 개구부 길이"),
        ("side_rail_thickness", "8 mm", "mm", "좌우 측면 레일 두께"),
        ("side_rail_height", "36 mm", "mm", "좌우 측면 레일 높이"),
        ("bumper_width", "290 mm", "mm", "앞뒤 범퍼 폭"),
        ("bumper_thickness", "12 mm", "mm", "앞뒤 범퍼 두께"),
        ("bumper_height", "32 mm", "mm", "앞뒤 범퍼 높이"),
        ("motor_pad_width", "82 mm", "mm", "교체형 모터 어댑터 패드 폭"),
        ("motor_pad_length", "28 mm", "mm", "교체형 모터 어댑터 패드 길이"),
        ("motor_pad_height", "8 mm", "mm", "모터 어댑터 패드 높이"),
        ("motor_pad_hole_x", "25 mm", "mm", "모터 패드 M3 홀 X 간격 반값"),
        ("motor_pad_hole_y", "7 mm", "mm", "모터 패드 M3 홀 Y 간격 반값"),
        ("m3_clearance_diameter", "3.4 mm", "mm", "M3 관통 여유홀 지름"),
        ("deck_width", "280 mm", "mm", "상부 데크 폭"),
        ("deck_length", "250 mm", "mm", "상부 데크 길이"),
        ("deck_thickness", "5 mm", "mm", "상부 데크 두께"),
        ("deck_post_diameter", "12 mm", "mm", "상부 데크 포스트 외경"),
        ("deck_post_height", "36 mm", "mm", "상부 데크 포스트 높이"),
        ("deck_post_x", "110 mm", "mm", "상부 데크 포스트 X 위치"),
        ("deck_post_y", "92 mm", "mm", "상부 데크 포스트 Y 위치"),
        ("arm_pad_diameter", "126 mm", "mm", "상부 작업 장치 인터페이스 패드 외경"),
        ("arm_pad_height", "6 mm", "mm", "상부 작업 장치 인터페이스 패드 높이"),
        ("arm_hole_x", "42 mm", "mm", "작업 장치 M3 홀 X 간격 반값"),
        ("arm_hole_y", "32 mm", "mm", "작업 장치 M3 홀 Y 간격 반값"),
        ("corner_radius", "18 mm", "mm", "주요 플레이트 외곽 모서리 라운드"),
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
                  x_mm=0.0, y_mm=0.0):
    sketch = component.sketches.add(component.xYConstructionPlane)
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


def extrude(component, sketch, distance_expr, operation, name):
    if sketch.profiles.count == 0:
        raise RuntimeError("프로파일을 찾을 수 없습니다: {}".format(sketch.name))
    input_def = component.features.extrudeFeatures.createInput(
        sketch.profiles.item(0),
        operation,
    )
    input_def.setDistanceExtent(
        False,
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
                  initial_height_mm, x_mm=0.0, y_mm=0.0, z_mm=0.0):
    occurrence, component = new_component(
        parent_component,
        name,
        x_mm,
        y_mm,
        z_mm,
        True,
        True,
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


def cut_circle(component, name, x_mm, y_mm, diameter_expr,
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


def cut_circles(component, name, points, diameter_expr, depth_expr,
                initial_diameter_mm=3.4):
    last_feature = None
    for index, (x_mm, y_mm) in enumerate(points, start=1):
        last_feature = cut_circle(
            component,
            "{}_{}".format(name, index),
            x_mm,
            y_mm,
            diameter_expr,
            initial_diameter_mm,
            depth_expr,
        )
    return last_feature


def make_cylinder_part(parent_component, name, diameter_expr, height_expr,
                       initial_diameter_mm, initial_height_mm,
                       x_mm=0.0, y_mm=0.0, z_mm=0.0):
    occurrence, component = new_component(
        parent_component,
        name,
        x_mm,
        y_mm,
        z_mm,
        True,
        True,
    )
    sketch = circle_sketch(
        component,
        name,
        diameter_expr,
        initial_diameter_mm,
    )
    extrude(
        component,
        sketch,
        height_expr,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        name + "_Extrude",
    )
    return occurrence, component


def safe_outer_fillet(component, radius_expr):
    # Keep the model usable if a particular Fusion release rejects a mixed
    # edge chain.  The fillet is a refinement, not a dependency of the frame.
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
            adsk.core.ValueInput.createByString(radius_expr),
            True,
        )
        feature = component.features.filletFeatures.add(fillet_input)
        feature.name = "01_Rounded_Outer_Profile"
        return feature
    except:
        return None


def make_lower_tub(assembly):
    occurrence, component = make_box_part(
        assembly,
        "01_ROVER_Lower_Tub_Plate",
        "rover_width",
        "rover_length",
        "base_thickness",
        320,
        420,
        6,
        0,
        0,
        0,
    )
    cut_rectangle(
        component,
        "01_Service_and_Wiring_Opening",
        "service_opening_width",
        "service_opening_length",
        150,
        260,
        "base_thickness",
    )
    perimeter_holes = [
        (-135, -190), (-45, -190), (45, -190), (135, -190),
        (-135, 190), (-45, 190), (45, 190), (135, 190),
        (-150, -120), (-150, 0), (-150, 120),
        (150, -120), (150, 0), (150, 120),
    ]
    cut_circles(
        component,
        "01_Lower_Tub_M3_Perimeter_Hole",
        perimeter_holes,
        "m3_clearance_diameter",
        "base_thickness",
    )
    safe_outer_fillet(component, "corner_radius")
    return occurrence


def make_side_structure(assembly):
    parts = []
    side_x = (-156, 156)
    for label, x_mm in (
        ("Left", side_x[0]),
        ("Right", side_x[1]),
    ):
        occurrence, _ = make_box_part(
            assembly,
            "01_ROVER_{}_Raised_Side_Rail".format(label),
            "side_rail_thickness",
            "rover_length - 50 mm",
            "side_rail_height",
            8,
            370,
            36,
            x_mm,
            0,
            6,
        )
        parts.append(occurrence)
    for label, y_mm in (
        ("Front", 204),
        ("Rear", -204),
    ):
        occurrence, _ = make_box_part(
            assembly,
            "01_ROVER_{}_Bumper_Beam".format(label),
            "bumper_width",
            "bumper_thickness",
            "bumper_height",
            290,
            12,
            32,
            0,
            y_mm,
            6,
        )
        parts.append(occurrence)
    return parts


def make_motor_interface_pad(assembly, label, x_mm, y_mm):
    occurrence, component = make_box_part(
        assembly,
        "02_ROVER_Motor_Interface_Pad_" + label,
        "motor_pad_width",
        "motor_pad_length",
        "motor_pad_height",
        82,
        28,
        8,
        x_mm,
        y_mm,
        6,
    )
    hole_points = [
        (-25, -7), (25, -7), (-25, 7), (25, 7),
    ]
    cut_circles(
        component,
        "02_Motor_Interface_M3_Hole_" + label,
        hole_points,
        "m3_clearance_diameter",
        "motor_pad_height",
    )
    return occurrence


def make_motor_interface_pads(assembly):
    parts = []
    for label, x_mm, y_mm in (
        ("FL", -115, -135),
        ("FR", 115, -135),
        ("RL", -115, 135),
        ("RR", 115, 135),
    ):
        parts.append(make_motor_interface_pad(assembly, label, x_mm, y_mm))
    return parts


def make_deck_posts(assembly):
    parts = []
    for label, x_mm, y_mm in (
        ("FL", -110, -92),
        ("FR", 110, -92),
        ("RL", -110, 92),
        ("RR", 110, 92),
    ):
        occurrence, component = make_cylinder_part(
            assembly,
            "03_ROVER_Upper_Deck_Post_" + label,
            "deck_post_diameter",
            "deck_post_height",
            12,
            36,
            x_mm,
            y_mm,
            6,
        )
        cut_circle(
            component,
            "03_Deck_Post_M3_" + label,
            0,
            0,
            "m3_clearance_diameter",
            3.4,
            "deck_post_height",
        )
        parts.append(occurrence)
    return parts


def make_upper_deck(assembly):
    occurrence, component = make_box_part(
        assembly,
        "03_ROVER_Upper_Electronics_Deck",
        "deck_width",
        "deck_length",
        "deck_thickness",
        280,
        250,
        5,
        0,
        0,
        42,
    )
    cut_rectangle(
        component,
        "03_Upper_Deck_Service_Opening",
        "service_opening_width - 20 mm",
        "service_opening_length - 50 mm",
        130,
        210,
        "deck_thickness",
    )
    deck_holes = [
        (-110, -92), (110, -92), (-110, 92), (110, 92),
        (-110, 0), (110, 0),
    ]
    cut_circles(
        component,
        "03_Upper_Deck_M3_Hole",
        deck_holes,
        "m3_clearance_diameter",
        "deck_thickness",
    )
    return occurrence


def make_arm_interface(assembly):
    occurrence, component = make_cylinder_part(
        assembly,
        "04_ROVER_Arm_Base_Interface",
        "arm_pad_diameter",
        "arm_pad_height",
        126,
        6,
        0,
        -12,
        47,
    )
    arm_holes = [
        (-42, -32), (42, -32), (-42, 32), (42, 32),
    ]
    cut_circles(
        component,
        "04_Arm_Interface_M3_Hole",
        arm_holes,
        "m3_clearance_diameter",
        "arm_pad_height",
    )
    return occurrence


def add_rigid_group(assembly, occurrences):
    try:
        collection = adsk.core.ObjectCollection.create()
        for occurrence in occurrences:
            collection.add(occurrence)
        group = assembly.rigidGroups.add(collection, False)
        group.name = "RG_ROVER_CHASSIS_Static_Structure"
        return group
    except:
        return None


def build_rover_chassis(design):
    root = design.rootComponent
    add_parameters(design)
    assembly_occurrence, assembly = new_component(
        root,
        "RC_CAR_ROVER_CHASSIS",
        0,
        0,
        0,
        True,
        True,
    )
    structural = [make_lower_tub(assembly)]
    structural.extend(make_side_structure(assembly))
    structural.extend(make_motor_interface_pads(assembly))
    structural.extend(make_deck_posts(assembly))
    structural.append(make_upper_deck(assembly))
    structural.append(make_arm_interface(assembly))
    add_rigid_group(assembly, structural)
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
        build_rover_chassis(design)
        try:
            app.activeViewport.fit()
        except:
            pass
        ui.messageBox(
            "RC_CAR 로버형 샤시만 생성했습니다.\n\n"
            "포함: 하부 차체, 좌우 레일, 범퍼, N20/TT 어댑터 패드 4개, "
            "상부 데크, 작업 장치 인터페이스\n"
            "미포함: 모터, 바퀴, 서보, ESP32, 드라이버, 배터리, STL\n\n"
            "Modify → Change Parameters에서 크기를 확인하세요."
        )
    except:
        if ui:
            ui.messageBox(
                "RC_CAR 로버형 샤시 생성 실패:\n{}".format(
                    traceback.format_exc()
                )
            )

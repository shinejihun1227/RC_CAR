"""
RC_CAR Reference Assembly Builder for Fusion 360

Purpose
-------
Create a simple, editable reference assembly so the RC_CAR chassis can be
modeled around the real component envelopes.  The solids are intentionally
simple envelopes, not manufacturing CAD for a specific seller.

Coordinate convention
---------------------
X = chassis width, Y = front/rear, Z = up.
The chassis is 300 x 400 x 5 mm.  Component positions are nominal and should
be adjusted after measuring the purchased parts.
"""

import adsk.core
import adsk.fusion
import traceback


MM_TO_CM = 0.1


def p3(x_mm, y_mm, z_mm=0.0):
    return adsk.core.Point3D.create(x_mm * MM_TO_CM, y_mm * MM_TO_CM, z_mm * MM_TO_CM)


def v3(x_mm, y_mm, z_mm=0.0):
    return adsk.core.Vector3D.create(x_mm * MM_TO_CM, y_mm * MM_TO_CM, z_mm * MM_TO_CM)


def new_component(parent_component, name, x_mm=0.0, y_mm=0.0, z_mm=0.0):
    transform = adsk.core.Matrix3D.create()
    transform.translation = v3(x_mm, y_mm, z_mm)
    occurrence = parent_component.occurrences.addNewComponent(transform)
    if not occurrence:
        raise RuntimeError("Occurrence 생성 실패: {}".format(name))
    occurrence.component.name = name
    try:
        occurrence.isGrounded = False
    except:
        pass
    return occurrence, occurrence.component


def ensure_parameter(design, name, expression, units, comment):
    parameter = design.userParameters.itemByName(name)
    if parameter:
        return parameter
    return design.userParameters.add(
        name,
        adsk.core.ValueInput.createByString(expression),
        units,
        comment,
    )


def add_parameters(design):
    values = [
        ("ref_chassis_width", "300 mm", "mm", "참조 샤시 폭"),
        ("ref_chassis_length", "400 mm", "mm", "참조 샤시 길이"),
        ("ref_chassis_thickness", "5 mm", "mm", "참조 샤시 두께"),
        ("ref_n20_length", "32.5 mm", "mm", "엔코더 포함 N20 길이"),
        ("ref_n20_width", "12 mm", "mm", "N20 폭"),
        ("ref_n20_height", "10 mm", "mm", "N20 높이"),
        ("ref_n20_shaft_diameter", "3 mm", "mm", "N20 출력축 지름"),
        ("ref_n20_shaft_length", "9 mm", "mm", "N20 출력축 길이"),
        ("ref_wheel_diameter", "70 mm", "mm", "바퀴 참조 지름"),
        ("ref_wheel_width", "22 mm", "mm", "바퀴 참조 폭"),
        ("ref_esp32_length", "55 mm", "mm", "ESP32 참조 길이"),
        ("ref_esp32_width", "28 mm", "mm", "ESP32 참조 폭"),
        ("ref_tb6612_length", "33 mm", "mm", "TB6612FNG 참조 길이"),
        ("ref_tb6612_width", "25 mm", "mm", "TB6612FNG 참조 폭"),
        ("ref_battery_length", "60 mm", "mm", "4칸 AA 홀더 참조 길이"),
        ("ref_battery_width", "32 mm", "mm", "4칸 AA 홀더 참조 폭"),
        ("ref_battery_height", "31 mm", "mm", "4칸 AA 홀더 참조 높이"),
        ("ref_fit_clearance", "0.4 mm", "mm", "FDM 조립 여유 시작값"),
    ]
    for name, expression, units, comment in values:
        ensure_parameter(design, name, expression, units, comment)


def rectangle_sketch(component, name, width_expr, length_expr, initial_width_mm, initial_length_mm):
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


def circle_sketch(component, name, diameter_expr, initial_diameter_mm):
    sketch = component.sketches.add(component.xYConstructionPlane)
    sketch.name = name + "_Sketch"
    circle = sketch.sketchCurves.sketchCircles.addByCenterRadius(
        p3(0, 0),
        (initial_diameter_mm / 2.0) * MM_TO_CM,
    )
    diameter_dim = sketch.sketchDimensions.addDiameterDimension(
        circle,
        p3(initial_diameter_mm / 2.0, 0),
    )
    diameter_dim.parameter.expression = diameter_expr
    return sketch


def circle_sketch_yz(component, name, diameter_expr, initial_diameter_mm):
    sketch = component.sketches.add(component.yZConstructionPlane)
    sketch.name = name + "_Sketch"
    circle = sketch.sketchCurves.sketchCircles.addByCenterRadius(
        adsk.core.Point3D.create(0, 0, 0),
        (initial_diameter_mm / 2.0) * MM_TO_CM,
    )
    diameter_dim = sketch.sketchDimensions.addDiameterDimension(
        circle,
        adsk.core.Point3D.create(0, (initial_diameter_mm / 2.0) * MM_TO_CM, 0),
    )
    diameter_dim.parameter.expression = diameter_expr
    return sketch


def extrude(component, sketch, distance_expr, operation, name):
    if sketch.profiles.count == 0:
        raise RuntimeError("프로파일을 찾을 수 없습니다: {}".format(sketch.name))
    input_def = component.features.extrudeFeatures.createInput(sketch.profiles.item(0), operation)
    input_def.setDistanceExtent(False, adsk.core.ValueInput.createByString(distance_expr))
    feature = component.features.extrudeFeatures.add(input_def)
    feature.name = name
    sketch.isVisible = False
    return feature


def add_box(component, name, width_expr, length_expr, height_expr, initial_width_mm, initial_length_mm, initial_height_mm):
    sketch = rectangle_sketch(component, name, width_expr, length_expr, initial_width_mm, initial_length_mm)
    return extrude(
        component,
        sketch,
        height_expr,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        name + "_Extrude",
    )


def add_cylinder(component, name, diameter_expr, height_expr, initial_diameter_mm):
    sketch = circle_sketch(component, name, diameter_expr, initial_diameter_mm)
    return extrude(
        component,
        sketch,
        height_expr,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        name + "_Extrude",
    )


def add_box_part(parent, name, width_expr, length_expr, height_expr, initial_width_mm, initial_length_mm, initial_height_mm, x_mm, y_mm, z_mm):
    _, component = new_component(parent, name, x_mm, y_mm, z_mm)
    add_box(component, name, width_expr, length_expr, height_expr, initial_width_mm, initial_length_mm, initial_height_mm)
    return component


def add_cylinder_part(parent, name, diameter_expr, height_expr, initial_diameter_mm, x_mm, y_mm, z_mm):
    _, component = new_component(parent, name, x_mm, y_mm, z_mm)
    add_cylinder(component, name, diameter_expr, height_expr, initial_diameter_mm)
    return component


def add_cylinder_axis_x_part(parent, name, diameter_expr, length_expr, initial_diameter_mm, x_mm, y_mm, z_mm):
    _, component = new_component(parent, name, x_mm, y_mm, z_mm)
    sketch = circle_sketch_yz(component, name, diameter_expr, initial_diameter_mm)
    extrude(
        component,
        sketch,
        length_expr,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        name + "_Extrude",
    )
    return component


def add_n20_reference(parent, label, x_mm, y_mm, z_mm):
    _, component = new_component(parent, "REF_N20_Encoder_Motor_" + label, x_mm, y_mm, z_mm)
    add_box(component, "REF_Motor_Body", "ref_n20_length", "ref_n20_width", "ref_n20_height", 32.5, 12, 10)
    add_box(component, "REF_Gearbox", "10 mm", "14 mm", "14 mm", 10, 14, 14)
    shaft = add_cylinder(component, "REF_Output_Shaft", "ref_n20_shaft_diameter", "ref_n20_shaft_length", 3)
    try:
        shaft.bodies.item(0).name = "REF_Output_Shaft_Body"
    except:
        pass
    add_box(component, "REF_Encoder_PCB", "4 mm", "14 mm", "3 mm", 4, 14, 3)
    return component


def add_wheel_reference(parent, label, x_mm, y_mm, z_mm):
    return add_cylinder_axis_x_part(parent, "REF_Wheel_" + label, "ref_wheel_diameter", "ref_wheel_width", 70, x_mm - 11, y_mm, z_mm)


def build_reference_assembly(design):
    add_parameters(design)
    root = design.rootComponent
    _, assembly = new_component(root, "RC_CAR_REFERENCE_ASSEMBLY")

    add_box_part(assembly, "00_CHASSIS_REFERENCE_BASE", "ref_chassis_width", "ref_chassis_length", "ref_chassis_thickness", 300, 400, 5, 0, 0, 0)

    motor_positions = [
        ("FL", -112, 135),
        ("FR", 112, 135),
        ("RL", -112, -135),
        ("RR", 112, -135),
    ]
    for label, x_mm, y_mm in motor_positions:
        add_box_part(assembly, "REF_N20_Bracket_" + label, "40 mm", "18 mm", "27.5 mm", 40, 18, 27.5, x_mm, y_mm, 5)
        add_n20_reference(assembly, label, x_mm, y_mm, 18)
        add_wheel_reference(assembly, label, x_mm, y_mm, 34)

    add_box_part(assembly, "REF_ESP32_DevKit_V1", "55 mm", "28 mm", "12 mm", 55, 28, 12, 0, 40, 20)
    add_box_part(assembly, "REF_TB6612FNG_Left", "33 mm", "25 mm", "8 mm", 33, 25, 8, -48, 40, 20)
    add_box_part(assembly, "REF_TB6612FNG_Right", "33 mm", "25 mm", "8 mm", 33, 25, 8, 48, 40, 20)
    add_box_part(assembly, "REF_AA_4Cell_Holder", "ref_battery_length", "ref_battery_width", "ref_battery_height", 60, 32, 31, 0, -70, 8)
    add_box_part(assembly, "REF_Buck_Boost_5V", "45 mm", "20 mm", "10 mm", 45, 20, 10, 0, -125, 20)
    add_box_part(assembly, "REF_VL53L1X_ToF", "21 mm", "17 mm", "6 mm", 21, 17, 6, 0, 188, 8)

    for label, x_mm, y_mm in (("FL", -110, 92), ("FR", 110, 92), ("RL", -110, -92), ("RR", 110, -92)):
        add_cylinder_part(assembly, "REF_M3_Deck_Spacer_" + label, "6.4 mm", "10 mm", 6.4, x_mm, y_mm, 12)

    return assembly


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
        build_reference_assembly(design)
        try:
            app.activeViewport.fit()
        except:
            pass
        ui.messageBox(
            "RC_CAR 참조 조립 배치를 생성했습니다.\n\n"
            "모든 형상은 배치·간섭 확인용 외곽 모델입니다.\n"
            "실제 모터, 바퀴, ESP32, 드라이버, 배터리 홀더를 측정한 뒤\n"
            "Modify → Change Parameters와 Move/Copy로 수정하세요."
        )
    except:
        if ui:
            ui.messageBox("RC_CAR 참조 조립 배치 생성 실패:\n{}".format(traceback.format_exc()))

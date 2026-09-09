"""
RC_CAR - Mounting Frame Builder for Fusion 360

Purpose
-------
Create a large, editable mounting frame for the RC_CAR project.  The visible
model is the chassis, rails, motor brackets, battery tray, upper deck, and
sensor mount.  Motors, wheels, controller STL files, and board envelopes are
created under a separate hidden reference occurrence so the design intent is
clear when the script finishes.

Nominal layout
--------------
  chassis: 300 x 400 x 5 mm with solid motor zones and a TT mounting bridge
  N20: four visible exchangeable motor mounting pods with open clamp cheeks
  TT: two hidden exchangeable mounting pods
  upper deck: 190 x 150 x 4 mm

The three local STL files that were measured for this design are represented
by editable reference bounding boxes:
  controller-bottom.STL       137.326 x 80.932 x 25 mm when laid flat
  controller-top.STL          137.326 x 80.932 x 21 mm when laid flat
  N20withEncoder_mount.stl     27 x 12 x 27.5 mm

These values are reference dimensions, not a guarantee that every purchased
part has the same interface.  Measure the actual motor, bracket, wheels,
controller, and battery holder before printing the final frame.
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
        ("chassis_width", "300 mm", "mm", "권장 대형 프레임 폭"),
        ("chassis_length", "400 mm", "mm", "권장 대형 프레임 길이"),
        ("base_thickness", "5 mm", "mm", "하부 프레임 두께"),
        ("relief_width", "140 mm", "mm", "중앙 경량화 개구부 폭"),
        ("relief_length", "240 mm", "mm", "중앙 경량화 개구부 길이"),
        ("side_rail_thickness", "6 mm", "mm", "좌우 보강 레일 두께"),
        ("side_rail_height", "24 mm", "mm", "좌우 보강 레일 높이"),
        ("bumper_width", "270 mm", "mm", "범퍼 폭"),
        ("bumper_thickness", "12 mm", "mm", "범퍼 앞뒤 두께"),
        ("bumper_height", "30 mm", "mm", "범퍼 높이"),
        ("mount_plate_thickness", "5 mm", "mm", "모터 장착판 두께"),
        ("mount_rail_thickness", "3 mm", "mm", "모터 가이드 레일 두께"),
        ("clamp_wall_thickness", "4 mm", "mm", "모터 클램프 벽 두께"),
        ("common_mount_hole_x", "30 mm", "mm", "N20/TT 공통 체결홀 X 오프셋"),
        ("common_mount_hole_y", "24 mm", "mm", "N20/TT 공통 체결홀 Y 오프셋"),
        ("tt_mount_bridge_length", "72 mm", "mm", "TT 교체 브래킷 지지 브리지 길이"),
        ("fit_clearance", "0.4 mm", "mm", "FDM 조립 여유 시작값"),
        ("n20_body_width", "12 mm", "mm", "N20 기준 폭"),
        ("n20_body_height", "10 mm", "mm", "N20 기준 높이"),
        ("n20_encoder_total_length", "32.5 mm", "mm", "엔코더 포함 N20 길이"),
        ("n20_shaft_diameter", "3 mm", "mm", "N20 축 외경"),
        ("n20_shaft_length", "9 mm", "mm", "N20 축 길이"),
        ("n20_mount_plate_width", "90 mm", "mm", "N20 공통 인터페이스 장착판 X"),
        ("n20_mount_plate_length", "64 mm", "mm", "N20 공통 인터페이스 장착판 Y"),
        ("n20_rail_length", "40 mm", "mm", "N20 가이드 길이"),
        ("n20_clamp_wall_offset", "8.5 mm", "mm", "N20 클램프 벽 중심 오프셋"),
        ("n20_clamp_wall_height", "16 mm", "mm", "N20 클램프 벽 높이"),
        ("tt_body_length", "70 mm", "mm", "TT 모터 기준 길이"),
        ("tt_body_width", "22 mm", "mm", "TT 모터 기준 폭"),
        ("tt_body_height", "19 mm", "mm", "TT 모터 기준 높이"),
        ("tt_shaft_diameter", "5.3 mm", "mm", "TT 축 기준 외경"),
        ("tt_shaft_length", "10 mm", "mm", "TT 축 기준 길이"),
        ("tt_mount_plate_width", "100 mm", "mm", "TT 공통 인터페이스 장착판 X"),
        ("tt_mount_plate_length", "72 mm", "mm", "TT 공통 인터페이스 장착판 Y"),
        ("tt_rail_length", "74 mm", "mm", "TT 가이드 길이"),
        ("tt_clamp_wall_offset", "13.5 mm", "mm", "TT 클램프 벽 중심 오프셋"),
        ("tt_clamp_wall_height", "26 mm", "mm", "TT 클램프 벽 높이"),
        ("n20_wheel_diameter", "42 mm", "mm", "N20 바퀴 참고 지름"),
        ("tt_wheel_diameter", "65 mm", "mm", "TT 바퀴 참고 지름"),
        ("battery_tray_width", "78 mm", "mm", "4xAA 트레이 폭"),
        ("battery_tray_length", "94 mm", "mm", "4xAA 트레이 길이"),
        ("tray_base_thickness", "3 mm", "mm", "배터리 트레이 바닥"),
        ("tray_wall_thickness", "3 mm", "mm", "배터리 트레이 벽 두께"),
        ("tray_wall_height", "22 mm", "mm", "배터리 트레이 벽 높이"),
        ("electronics_deck_width", "190 mm", "mm", "상부 장착 데크 폭"),
        ("electronics_deck_length", "150 mm", "mm", "상부 장착 데크 길이"),
        ("deck_thickness", "4 mm", "mm", "상부 데크 두께"),
        ("deck_standoff_height", "31 mm", "mm", "상부 데크 지지 높이"),
        ("controller_stl_width", "137.326 mm", "mm", "controller STL X 바운딩 박스"),
        ("controller_stl_depth", "80.932 mm", "mm", "controller STL Y 바운딩 박스"),
        ("controller_stl_height", "25 mm", "mm", "controller-bottom STL 높이"),
        ("controller_top_stl_height", "21 mm", "mm", "controller-top STL 높이"),
        ("n20_mount_stl_width", "27 mm", "mm", "N20 mount STL X 바운딩 박스"),
        ("n20_mount_stl_depth", "12 mm", "mm", "N20 mount STL Y 바운딩 박스"),
        ("n20_mount_stl_height", "27.5 mm", "mm", "N20 mount STL Z 바운딩 박스"),
        ("tof_plate_width", "54 mm", "mm", "ToF 센서 브래킷 폭"),
        ("tof_plate_height", "44 mm", "mm", "ToF 센서 브래킷 높이"),
    ]
    for name, expression, units, comment in parameters:
        ensure_parameter(design, name, expression, units, comment)


def rectangle_sketch(component, name, width_expr, length_expr,
                     initial_width_mm, initial_length_mm):
    sketch = component.sketches.add(component.xYConstructionPlane)
    sketch.name = name + "_Sketch"
    hw = initial_width_mm / 2.0
    hl = initial_length_mm / 2.0
    lines = sketch.sketchCurves.sketchLines.addTwoPointRectangle(
        p3(-hw, -hl), p3(hw, hl)
    )
    width_dim = sketch.sketchDimensions.addDistanceDimension(
        lines.item(0).startSketchPoint,
        lines.item(0).endSketchPoint,
        adsk.fusion.DimensionOrientations.HorizontalDimensionOrientation,
        p3(0, -hl - 10),
    )
    width_dim.parameter.expression = width_expr
    length_dim = sketch.sketchDimensions.addDistanceDimension(
        lines.item(1).startSketchPoint,
        lines.item(1).endSketchPoint,
        adsk.fusion.DimensionOrientations.VerticalDimensionOrientation,
        p3(hw + 10, 0),
    )
    length_dim.parameter.expression = length_expr
    return sketch


def extrude(component, sketch, distance_expr, operation, name,
            symmetric=False):
    if sketch.profiles.count == 0:
        raise RuntimeError("프로파일을 찾을 수 없습니다: {}".format(sketch.name))
    input_def = component.features.extrudeFeatures.createInput(
        sketch.profiles.item(0), operation
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
                  initial_height_mm, x_mm=0, y_mm=0, z_mm=0,
                  visible=True, grounded=False):
    # Always build visible.  Hide only after the body exists; this avoids the
    # Fusion error where a feature cannot find its target body.
    occurrence, component = new_component(
        parent_component, name, x_mm, y_mm, z_mm, True, grounded
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
    set_visibility(occurrence, visible)
    return occurrence, component


def cut_one_circle(component, name, x_mm, y_mm, radius_mm,
                   depth_expr, plane=None):
    sketch = component.sketches.add(
        plane if plane else component.xYConstructionPlane
    )
    sketch.name = name + "_Sketch"
    circle = sketch.sketchCurves.sketchCircles.addByCenterRadius(
        p3(x_mm, y_mm), radius_mm * MM_TO_CM
    )
    if sketch.profiles.count == 0:
        raise RuntimeError("원형 프로파일을 찾을 수 없습니다: {}".format(name))
    input_def = component.features.extrudeFeatures.createInput(
        sketch.profiles.item(0),
        adsk.fusion.FeatureOperations.CutFeatureOperation,
    )
    input_def.setDistanceExtent(
        False,
        adsk.core.ValueInput.createByString(depth_expr),
    )
    feature = component.features.extrudeFeatures.add(input_def)
    feature.name = name + "_Cut"
    sketch.isVisible = False
    return feature


def cut_circles(component, name, depth_expr, radius_mm, points):
    # One sketch and one Cut feature per hole is more reliable than a single
    # multi-profile cut, especially when the part is later edited.
    last_feature = None
    for index, (x_mm, y_mm) in enumerate(points, start=1):
        last_feature = cut_one_circle(
            component,
            "{}_{}".format(name, index),
            x_mm,
            y_mm,
            radius_mm,
            depth_expr,
        )
    return last_feature


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


def make_plate_part(parent_component, name, width_expr, length_expr,
                    thickness_expr, initial_width_mm, initial_length_mm,
                    initial_thickness_mm, hole_x_mm, hole_y_mm,
                    x_mm=0, y_mm=0, z_mm=0, visible=True, grounded=False):
    occurrence, component = make_box_part(
        parent_component,
        name,
        width_expr,
        length_expr,
        thickness_expr,
        initial_width_mm,
        initial_length_mm,
        initial_thickness_mm,
        x_mm,
        y_mm,
        z_mm,
        True,
        grounded,
    )
    cut_circles(
        component,
        name + "_M3_Clearance_Hole",
        thickness_expr,
        1.7,
        [
            (-hole_x_mm, -hole_y_mm),
            (hole_x_mm, -hole_y_mm),
            (-hole_x_mm, hole_y_mm),
            (hole_x_mm, hole_y_mm),
        ],
    )
    set_visibility(occurrence, visible)
    return occurrence, component


def make_lower_frame(assembly):
    occurrence, component = new_component(
        assembly, "01_FRAME_Lower_Chassis", 0, 0, 0, True, True
    )
    add_box(component, "01_Base_Plate", "chassis_width", "chassis_length",
            "base_thickness", 300, 400, 5)
    cut_rectangle(
        component,
        "01_Center_Lightening_Relief",
        "relief_width",
        "relief_length",
        140,
        240,
        "base_thickness",
    )
    cut_circles(
        component,
        "01_Perimeter_M3_Clearance_Hole",
        "base_thickness",
        1.7,
        [
            (-130, -180), (-65, -180), (65, -180), (130, -180),
            (-130, 180), (-65, 180), (65, 180), (130, 180),
            (-140, -110), (-140, 0), (-140, 110),
            (140, -110), (140, 0), (140, 110),
            (-135, -174), (-75, -174), (-135, -126), (-75, -126),
            (75, -174), (135, -174), (75, -126), (135, -126),
            (-135, 126), (-75, 126), (-135, 174), (-75, 174),
            (75, 126), (135, 126), (75, 174), (135, 174),
        ],
    )
    set_visibility(occurrence, True)
    return occurrence


def make_frame_rails(assembly):
    parts = []
    for name, x_mm in (
        ("01_FRAME_Left_Side_Rail", -147),
        ("01_FRAME_Right_Side_Rail", 147),
    ):
        occurrence, _ = make_box_part(
            assembly,
            name,
            "side_rail_thickness",
            "chassis_length - 20 mm",
            "side_rail_height",
            6,
            380,
            24,
            x_mm,
            0,
            5,
            True,
            True,
        )
        parts.append(occurrence)

    for name, y_mm in (
        ("01_FRAME_Front_Bumper", 194),
        ("01_FRAME_Rear_Bumper", -194),
    ):
        occurrence, _ = make_box_part(
            assembly,
            name,
            "bumper_width",
            "bumper_thickness",
            "bumper_height",
            270,
            12,
            30,
            0,
            y_mm,
            5,
            True,
            True,
        )
        parts.append(occurrence)

    for name, y_mm in (
        ("01_FRAME_UpperDeck_Support_Front", -20),
        ("01_FRAME_UpperDeck_Support_Rear", 90),
    ):
        occurrence, _ = make_box_part(
            assembly,
            name,
            "chassis_width - 60 mm",
            "8 mm",
            "8 mm",
            240,
            8,
            8,
            0,
            y_mm,
            5,
            True,
            True,
        )
        parts.append(occurrence)
    return parts


def make_tt_mounting_bridge(assembly):
    # The center of the lower plate is lightened, so the hidden TT option gets
    # a dedicated solid bridge.  The bridge uses the same four-hole pattern as
    # the N20 plates, which is the common mechanical interface.
    occurrence, component = make_box_part(
        assembly,
        "01_FRAME_TT_Mounting_Bridge",
        "chassis_width - 20 mm",
        "tt_mount_bridge_length",
        "base_thickness",
        280,
        72,
        5,
        0,
        0,
        5,
        True,
        True,
    )
    cut_circles(
        component,
        "01_TT_Bridge_M3_Clearance_Hole",
        "base_thickness",
        1.7,
        [
            (-135, -24), (-75, -24),
            (75, -24), (135, -24),
            (-135, 24), (-75, 24),
            (75, 24), (135, 24),
        ],
    )
    return occurrence


def make_motor_bracket(assembly, mode, label, x_mm, y_mm, visible):
    is_n20 = mode == "N20"
    if is_n20:
        prefix = "02_N20_Bracket_"
        plate_w_expr = "n20_mount_plate_width"
        plate_l_expr = "n20_mount_plate_length"
        plate_w = 58
        plate_l = 54
        hole_x = 30
        hole_y = 24
        rail_expr = "n20_rail_length"
        rail_initial = 40
        wall_offset_expr = "n20_clamp_wall_offset"
        wall_offset_initial = 8.5
        rail_height_expr = "n20_clamp_wall_height"
        rail_height_initial = 8
    else:
        prefix = "03_TT_Bracket_"
        plate_w_expr = "tt_mount_plate_width"
        plate_l_expr = "tt_mount_plate_length"
        plate_w = 96
        plate_l = 58
        hole_x = 30
        hole_y = 24
        rail_expr = "tt_rail_length"
        rail_initial = 74
        wall_offset_expr = "tt_clamp_wall_offset"
        wall_offset_initial = 13.5
        rail_height_expr = "tt_clamp_wall_height"
        rail_height_initial = 26

    occurrence, bracket = make_plate_part(
        assembly,
        prefix + label,
        plate_w_expr,
        plate_l_expr,
        "mount_plate_thickness",
        plate_w,
        plate_l,
        5,
        hole_x,
        hole_y,
        x_mm,
        y_mm,
        5,
        True,
        True,
    )
    # The wire window leaves room for the encoder cable and keeps the motor
    # from being trapped against a solid plate.
    cut_rectangle(
        bracket,
        prefix + "Wire_Relief_" + label,
        "18 mm",
        "8 mm",
        18,
        8,
        "mount_plate_thickness",
    )

    # These are open clamp cheeks rather than generic low rails.  The motor
    # slides in from the shaft direction; the encoder side remains open.
    set_visibility(occurrence, True)
    for side, local_y in (("Left", -wall_offset_initial), ("Right", wall_offset_initial)):
        make_box_part(
            bracket,
            prefix + "ClampWall_" + side + "_" + label,
            rail_expr,
            "clamp_wall_thickness",
            rail_height_expr,
            rail_initial,
            4,
            rail_height_initial,
            0,
            local_y,
            4,
            True,
            True,
        )
    set_visibility(occurrence, visible)
    return occurrence


def make_upper_deck(assembly):
    deck_occurrence, _ = make_plate_part(
        assembly,
        "04_FRAME_Upper_Electronics_Deck",
        "electronics_deck_width",
        "electronics_deck_length",
        "deck_thickness",
        190,
        150,
        4,
        80,
        55,
        0,
        35,
        36,
        True,
        True,
    )
    for label, x_mm, y_mm in (
        ("FL", -80, -20),
        ("FR", 80, -20),
        ("RL", -80, 90),
        ("RR", 80, 90),
    ):
        make_cylinder_part(
            assembly,
            "04_FRAME_Deck_Standoff_" + label,
            "6 mm",
            "deck_standoff_height",
            6,
            31,
            x_mm,
            y_mm,
            5,
            True,
            True,
        )
    return deck_occurrence


def make_cylinder_part(parent_component, name, diameter_expr, height_expr,
                       initial_diameter_mm, initial_height_mm,
                       x_mm, y_mm, z_mm, visible=True, grounded=False):
    occurrence, component = new_component(
        parent_component, name, x_mm, y_mm, z_mm, True, grounded
    )
    sketch = component.sketches.add(component.xYConstructionPlane)
    sketch.name = name + "_Sketch"
    circle = sketch.sketchCurves.sketchCircles.addByCenterRadius(
        p3(0, 0), (initial_diameter_mm / 2.0) * MM_TO_CM
    )
    diameter_dim = sketch.sketchDimensions.addDiameterDimension(
        circle, p3(initial_diameter_mm / 2.0, 0)
    )
    diameter_dim.parameter.expression = diameter_expr
    extrude(
        component,
        sketch,
        height_expr,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        name + "_Extrude",
    )
    set_visibility(occurrence, visible)
    return occurrence


def make_battery_tray(assembly):
    occurrence, tray = make_plate_part(
        assembly,
        "05_FRAME_4xAA_Battery_Tray",
        "battery_tray_width",
        "battery_tray_length",
        "tray_base_thickness",
        78,
        94,
        3,
        28,
        38,
        0,
        -72,
        5,
        True,
        True,
    )
    for name, x_mm, y_mm, w_expr, l_expr, iw, il in (
        ("Left_Wall", -37.5, 0, "tray_wall_thickness", "battery_tray_length", 3, 94),
        ("Right_Wall", 37.5, 0, "tray_wall_thickness", "battery_tray_length", 3, 94),
        ("Front_Stop", 0, 45.5, "battery_tray_width", "tray_wall_thickness", 78, 3),
        ("Rear_Stop", 0, -45.5, "battery_tray_width", "tray_wall_thickness", 78, 3),
    ):
        make_box_part(
            tray,
            "05_Battery_" + name,
            w_expr,
            l_expr,
            "tray_wall_height",
            iw,
            il,
            22,
            x_mm,
            y_mm,
            3,
            True,
            True,
        )
    set_visibility(occurrence, True)
    return occurrence


def make_sensor_mount(assembly):
    plate, _ = make_box_part(
        assembly,
        "06_FRAME_ToF_Sensor_Mount",
        "tof_plate_width",
        "4 mm",
        "tof_plate_height",
        54,
        4,
        44,
        0,
        185,
        5,
        True,
        True,
    )
    return plate


def make_reference_layout(assembly):
    occurrence, refs = new_component(
        assembly,
        "90_REFERENCE_STL_and_Electronics_Hidden",
        0,
        0,
        0,
        False,
        False,
    )
    references = [
        (
            "90_REF_controller-bottom_STL_BBox",
            "controller_stl_width",
            "controller_stl_depth",
            "controller_stl_height",
            137.326,
            80.932,
            25,
            0,
            35,
            40,
        ),
        (
            "90_REF_controller-top_STL_BBox",
            "controller_stl_width",
            "controller_stl_depth",
            "controller_top_stl_height",
            137.326,
            80.932,
            21,
            0,
            35,
            65,
        ),
        (
            "90_REF_N20withEncoder_mount_STL_BBox",
            "n20_mount_stl_width",
            "n20_mount_stl_depth",
            "n20_mount_stl_height",
            27,
            12,
            27.5,
            -115,
            35,
            40,
        ),
        (
            "90_REF_ESP32_Envelope",
            "26 mm",
            "52 mm",
            "5 mm",
            26,
            52,
            5,
            -42,
            40,
            40,
        ),
        (
            "90_REF_TB6612FNG_Envelope_A",
            "24 mm",
            "21 mm",
            "4 mm",
            24,
            21,
            4,
            20,
            20,
            40,
        ),
        (
            "90_REF_TB6612FNG_Envelope_B",
            "24 mm",
            "21 mm",
            "4 mm",
            24,
            21,
            4,
            20,
            55,
            40,
        ),
    ]
    for name, w_expr, l_expr, h_expr, iw, il, ih, x_mm, y_mm, z_mm in references:
        make_box_part(
            refs,
            name,
            w_expr,
            l_expr,
            h_expr,
            iw,
            il,
            ih,
            x_mm,
            y_mm,
            z_mm,
            False,
            False,
        )
    return occurrence


def make_circular_reference(assembly, mode, label, x_mm, y_mm, z_mm):
    is_n20 = mode == "N20"
    if is_n20:
        name = "90_REF_N20_Wheel_" + label
        diameter_expr = "n20_wheel_diameter"
        width_expr = "18 mm"
        initial_diameter = 42
        initial_width = 18
    else:
        name = "90_REF_TT_Wheel_" + label
        diameter_expr = "tt_wheel_diameter"
        width_expr = "25 mm"
        initial_diameter = 65
        initial_width = 25
    occurrence, component = new_component(
        assembly, name, x_mm, y_mm, z_mm, True, False
    )
    sketch = component.sketches.add(component.yZConstructionPlane)
    sketch.name = name + "_Sketch"
    circle = sketch.sketchCurves.sketchCircles.addByCenterRadius(
        p3(0, 0), initial_diameter / 2.0 * MM_TO_CM
    )
    diameter_dim = sketch.sketchDimensions.addDiameterDimension(
        circle, p3(initial_diameter / 2.0, 0)
    )
    diameter_dim.parameter.expression = diameter_expr
    extrude(
        component,
        sketch,
        width_expr,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        name + "_Extrude",
        True,
    )
    set_visibility(occurrence, False)
    return occurrence


def add_static_rigid_group(assembly, occurrences):
    if len(occurrences) < 2:
        return None
    try:
        collection = adsk.core.ObjectCollection.create()
        for occurrence in occurrences:
            collection.add(occurrence)
        group = assembly.rigidGroups.add(collection, False)
        group.name = "RG_Static_Mounting_Frame"
        return group
    except:
        return None


def build_assembly(design):
    root = design.rootComponent
    add_parameters(design)
    assembly_occurrence, assembly = new_component(
        root,
        "RC_CAR_MOUNTING_ASSEMBLY",
        0,
        0,
        0,
        True,
        True,
    )
    structural = [make_lower_frame(assembly)]
    structural.extend(make_frame_rails(assembly))
    structural.append(make_tt_mounting_bridge(assembly))

    # N20 is the primary visible layout.  The mounting frame remains visible;
    # the motor/wheel reference geometry is hidden in 90_REFERENCE.
    for label, x_mm, y_mm in (
        ("FL", -105, 150),
        ("FR", 105, 150),
        ("RL", -105, -150),
        ("RR", 105, -150),
    ):
        structural.append(
            make_motor_bracket(assembly, "N20", label, x_mm, y_mm, True)
        )
        make_circular_reference(
            assembly,
            "N20",
            label,
            -157 if x_mm < 0 else 157,
            y_mm,
            21,
        )

    # TT is kept as a replacement option and hidden by default.
    for label, x_mm in (("Left", -105), ("Right", 105)):
        structural.append(
            make_motor_bracket(assembly, "TT", label, x_mm, 0, False)
        )
        make_circular_reference(
            assembly,
            "TT",
            label,
            -157 if x_mm < 0 else 157,
            0,
            32.5,
        )

    structural.append(make_battery_tray(assembly))
    structural.append(make_upper_deck(assembly))
    structural.append(make_sensor_mount(assembly))
    make_reference_layout(assembly)
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
        build_assembly(design)
        try:
            app.activeViewport.fit()
        except:
            pass
        ui.messageBox(
            "RC_CAR 장착 프레임을 생성했습니다.\n\n"
            "표시: 하부 프레임, 레일, N20 브래킷, 배터리 트레이, 상부 데크, ToF 거치대\n"
            "숨김: TT 브래킷, 모터·바퀴·local STL reference\n\n"
            "Modify → Change Parameters에서 실제 치수를 입력하세요."
        )
    except:
        if ui:
            ui.messageBox(
                "RC_CAR 장착 프레임 생성 실패:\n{}".format(
                    traceback.format_exc()
                )
            )

object formSlider: TformSlider
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'Slider'
  ClientHeight = 85
  ClientWidth = 800
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Visible = True
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object panelSlider: TPanel
    Left = 0
    Top = 0
    Width = 800
    Height = 85
    Align = alClient
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    object btnShowHide: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 8
      Height = 77
      Cursor = crSizeWE
      Hint = 'Slider'
      Align = alLeft
      TabOrder = 0
      OnClick = btnShowHideClick
    end
    object gboxSlider: TGroupBox
      Left = 15
      Top = 1
      Width = 784
      Height = 83
      Align = alClient
      TabOrder = 1
      OnClick = gboxSliderClick
      DesignSize = (
        784
        83)
      object imageEye: TImage
        Left = 471
        Top = 22
        Width = 23
        Height = 21
        Center = True
        Picture.Data = {
          0954506E67496D61676589504E470D0A1A0A0000000D49484452000000180000
          00180806000000E0773DF80000000467414D410000AFC837058AE90000001974
          455874536F6674776172650041646F626520496D616765526561647971C9653C
          000004114944415478DAED556D4C5B55187ECA6DE907B7B7B4657C94B608E5A3
          084C1C02119CDB40C79638A741D12D0E16E322266409A2FCD09025A2FE5B8253
          FC6316602C2111DD50928DC154500930028C92F1E54A2AED6CA18596D2D222A5
          F5DCE6629C4EF7CB18939DE4C9C939E7BECFF3BEEF79CF7B79A15008FFE6E03D
          10F8EF05FEEEC075ADA9643B4E554A89A5FB25B4B228E8F1231404668C66087D
          4E88031E08DD56D867C6466DF6E56B9DD3CED6F313760B31FD9520744F01D750
          4B0936375F8D148B8F0834390C159386755F108C340ADB5B2EB4B6B4E1E1CC6C
          5CBA7C85CC7A94BF7C1CB48007E77717C3B02CAF5EF96864A9BE6BCEB940E8FC
          AC50586065BA4B1EB9E93BC7A724CF52229AE12715E2C6F814A66F4DA1B7E72A
          A4517C6C88D4989D3361DDED4528E0475E0271928AC49977DE86BE703FB6DC2B
          98683ABD6CFBC930BFECDDFAE254B7A98D507BC2028EE1F6D295B58DBDED9FF7
          541A5D48660D478687211289909EA14720BF0A3E8102093134E2E3A4F06C6C62
          61CE88C5A9111447BBF1B44E80F2CAD7C15765A2AFFBD26C63CDC9CF7E585CBF
          4AA84D3B29124E5E38D3A0DA15FDEE5786157CD8DC8E587D216A6ADFC2BE4792
          D13A60C35A9087DD3A054E142660F7073D284994C21FA4316F5A4463590CB4BE
          DB900929307B8FA3B3B3B3BFA2A2A281F04EF29235DA3D69EA98B4DA179F3C3B
          32634EECBB318D833A192C2FB5A0BA5803751485F72E4E81120B10218EC4E0C4
          04747C1F98C41494E6A5235B2545A69681C7BB81B5EBE711615F40FC2B8DA024
          4C1511E86323E0CB6532B548283A29120AEBCF9E2812EFD1A760F6F17A94A5CA
          C2E1BDF9E928940A094C2B3E6C6DAC86F7041205EA8EA643AF667E2F92603088
          DBA77321CA3E80A437CEBD4FB63AD8085EDB0E0613288AAA2FCAD22E917074F9
          196A08F39F872EBB004A3185A15B76740F99A189A7316E74C2EB0F4048F1F049
          4D01A244FCBBCA7BEC540E5CC2383CD5FC4D1359B6F192D4EAEA0C4DECA186AA
          83479B2F0F863F5229195496E54194B50FFC870AA053D2301857717DCC8A25A7
          0FB9A90AE4A4C8919D2CFFCBFB191F1F77D6D5D5DDECEFEFEF23CB2FD914490F
          3C9699AB9447579BAD0E893F10F11CFB3A126364F8B8B61CB1B1BBE06074D0AA
          B48890A9484223EF22F4783C81F9F9F9F5DEDEDE3B1D1D1D4683C1F0335B3DAC
          56F892D93B20601399A8888E7E262B23E598DBE3CFD1A76BA14FD5A2F2481139
          A081C016268DABCED119B3DDE25873996C4ED7C0C080CD6AB5AE115B2FFB9C08
          7E213073F332BBBF53A6ECCCBAC630345D7CACE2F00B870F3D51F268AE5ED6F5
          F5B796EF7F1CBB333874D3625B723838433B014BECE6E0E5B0CE6173A75DFCB9
          17515C341A8234022D27ECE0BC62BD74B299E14876B0FDC7FE73BF66B7130DCD
          89519C772CA9EF9FC8EE29F0FFFF1F3C10B8DFF80DA3A89ACF47AB1231000000
          0049454E44AE426082}
        OnMouseDown = imageEyeMouseDown
        OnMouseUp = imageEyeMouseUp
      end
      object stbarStatus: TStatusBar
        Left = 2
        Top = 62
        Width = 780
        Height = 19
        Panels = <>
        SimplePanel = True
      end
      object leditKey: TLabeledEdit
        Left = 496
        Top = 22
        Width = 267
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 18
        EditLabel.Height = 13
        EditLabel.Caption = 'Key'
        PasswordChar = '*'
        TabOrder = 1
        TextHint = 'Enter your password'
      end
      object cboxCheckOnlyMode: TCheckBox
        Left = 19
        Top = 24
        Width = 107
        Height = 17
        Caption = 'Check Only Mode'
        TabOrder = 2
      end
      object cboxProcessFirstRow: TCheckBox
        Left = 128
        Top = 24
        Width = 108
        Height = 17
        Caption = 'Process First Row'
        TabOrder = 3
      end
      object cboxPasswordStrength: TCheckBox
        Left = 348
        Top = 24
        Width = 113
        Height = 17
        Caption = 'Password Strength'
        TabOrder = 4
      end
      object pbarProgress: TProgressBar
        Left = 2
        Top = 54
        Width = 780
        Height = 8
        Align = alBottom
        Smooth = True
        Step = 1
        TabOrder = 5
      end
      object cboxProcessFirstCol: TCheckBox
        Left = 238
        Top = 24
        Width = 108
        Height = 17
        Caption = 'Process First Col'
        TabOrder = 6
      end
    end
  end
end

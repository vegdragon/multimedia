//
//  ViewController.swift
//  MusicAssistant
//
//  Created by Wu Jian on 10/13/15.
//  Copyright © 2015 Baresi. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


    @IBOutlet weak var detectedMusic: UITableView!
    @IBOutlet weak var toolBar: UIToolbar!
    @IBOutlet weak var recordButton: UIBarButtonItem!
    
    @IBAction func startRecording(sender: UIBarButtonItem)
    {
        print("hello")
        AQRecorderObjC.initRecorder()
        AQRecorderObjC.startRecord()
        
    }
    
    override func awakeFromNib()
    {
        print("awakeFromNib")
        AQRecorderObjC.initialize()
        
        // Allocate our singleton instance for the recorder & player object
        
//        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playbackQueueStopped:) name:@"playbackQueueStopped" object:nil];
//        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playbackQueueResumed:) name:@"playbackQueueResumed" object:nil];
//        
//
//        
//        [self registerForBackgroundNotifications];

    }
    
    

}



